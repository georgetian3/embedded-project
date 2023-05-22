#include <gtk/gtk.h>
#include "audioplayer.h"

typedef struct {
    AudioPlayer* ap;    
    GtkAdjustment* slider_adjustment;
    GtkAdjustment* volume_adjustment;
    GtkWidget* timestamp_label;
} AudioPlayerGUI;

typedef struct {
    int seconds;
    int minutes;
    int hours;
} Duration;

#define error_print(expr) int __ret; if (__ret = expr) {g_print("%s\n", ap_errors[__ret]);}

static Duration seconds_to_duration(int seconds) {
    Duration duration;
    duration.seconds = seconds % 60;
    seconds /= 60;
    duration.minutes = seconds % 60;
    seconds /= 60;
    duration.hours = seconds > 99 ? 99 : seconds;
    return duration;
}

static void gtk_button_set_icon(GtkWidget* button, const gchar *icon_name, GtkIconSize size) {
    GtkWidget *image = gtk_image_new_from_icon_name(icon_name, size);
    gtk_button_set_image((GtkButton*)button, image);
}


static void play_pause_clicked(GtkWidget* widget, gpointer data) {
    g_print("Play/pause clicked\n");
    AudioPlayerGUI* ap_gui = (AudioPlayerGUI*)data;
    error_print(ap_play_pause(ap_gui->ap));
    g_print("is_playing: %d\n", ap_is_playing(ap_gui->ap));
    gtk_button_set_icon(widget, ap_is_playing(ap_gui->ap) ? "media-playback-pause" : "media-playback-start", GTK_ICON_SIZE_BUTTON);
}

static void slider_released(GtkWidget* widget, gpointer data) {
    g_print("Slider released\n");
    AudioPlayerGUI* ap_gui = (AudioPlayerGUI*)data;
    double slider_value = (double)gtk_adjustment_get_value(ap_gui->slider_adjustment);
    ap_set_timestamp(ap_gui->ap, slider_value);
}

static void set_timestamp_text(GtkWidget* widget, gpointer data) {
    g_print("Setting timestamp text\n");
    AudioPlayerGUI* ap_gui = (AudioPlayerGUI*)data;
    Duration timestamp = seconds_to_duration(ap_get_timestamp(ap_gui->ap));
    Duration duration = seconds_to_duration(ap_duration(ap_gui->ap));
    char timestamp_buffer[18];
    sprintf(timestamp_buffer, "%02d:%02d:%02d/%02d:%02d:%02d", 
        timestamp.hours, timestamp.minutes, timestamp.seconds, 
        duration.hours, duration.minutes, duration.seconds
    );
    gtk_label_set_text((GtkLabel*)ap_gui->timestamp_label, timestamp_buffer);
    ap_scan_dir(ap_gui->ap, "./");
    int file_count = ap_audio_file_count(ap_gui->ap);
    const char** files = ap_get_audio_files(ap_gui->ap);
    for (int i = 0; i < file_count; i++) {
        g_print("%d: %s\n", i + 1, files[i]);
    }
    error_print(ap_open(ap_gui->ap, files[0]));
}

static void activate(GtkApplication* app, gpointer data) {

    AudioPlayerGUI* ap_gui = (AudioPlayerGUI*)data;

    GtkWidget* window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Audio Player");
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);

    GtkWidget* vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    GtkWidget* playlist_scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(vbox), playlist_scroll);
    gtk_widget_set_vexpand(playlist_scroll, TRUE);

    GtkWidget* playlist = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
    gtk_container_add(GTK_CONTAINER(playlist_scroll), playlist);
    gtk_scrolled_window_set_min_content_height((GtkScrolledWindow*)playlist_scroll, 200);

    for (int i = 0; i < 10; i++) {
        GtkWidget* button1 = gtk_button_new_with_label("Button 2");
        gtk_container_add(GTK_CONTAINER(playlist), button1);
    }

    GtkWidget* slider_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
    gtk_container_add(GTK_CONTAINER(vbox), slider_box);

    ap_gui->slider_adjustment = gtk_adjustment_new(0, 0, 10000, 10, 0, 0);
    GtkWidget* slider = gtk_scale_new(GTK_ORIENTATION_HORIZONTAL, ap_gui->slider_adjustment);
    gtk_container_add(GTK_CONTAINER(slider_box), slider);
    gtk_scale_set_draw_value((GtkScale*)slider, FALSE);
    gtk_widget_set_hexpand(slider, TRUE);
    g_signal_connect(slider, "button-release-event", G_CALLBACK(slider_released), ap_gui);


    //ss_to_hh_mm_ss(timestamp_buffer + 9, duration);

    ap_gui->timestamp_label = gtk_label_new("00:00:00/00:00:00");
    gtk_container_add(GTK_CONTAINER(slider_box), ap_gui->timestamp_label);

    GtkWidget* controls = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
    gtk_widget_set_halign(controls, GTK_ALIGN_CENTER);
    gtk_container_add(GTK_CONTAINER(vbox), controls);

    GtkWidget* repeat = gtk_button_new_from_icon_name("media-playlist-repeat", GTK_ICON_SIZE_BUTTON);
    gtk_container_add(GTK_CONTAINER(controls), repeat);

    GtkWidget* skip_backward = gtk_button_new_from_icon_name("media-skip-backward", GTK_ICON_SIZE_BUTTON);
    gtk_container_add(GTK_CONTAINER(controls), skip_backward);

    GtkWidget* seek_backward = gtk_button_new_from_icon_name("media-seek-backward", GTK_ICON_SIZE_BUTTON);
    gtk_container_add(GTK_CONTAINER(controls), seek_backward);

    GtkWidget* play_pause = gtk_button_new_from_icon_name("media-playback-start", GTK_ICON_SIZE_BUTTON);
    gtk_container_add(GTK_CONTAINER(controls), play_pause);
    g_signal_connect(play_pause, "clicked", G_CALLBACK(play_pause_clicked), ap_gui);

    GtkWidget* seek_forward = gtk_button_new_from_icon_name("media-seek-forward", GTK_ICON_SIZE_BUTTON);
    gtk_container_add(GTK_CONTAINER(controls), seek_forward);
    g_signal_connect(seek_forward, "clicked", G_CALLBACK(set_timestamp_text), ap_gui);

    GtkWidget* skip_forward = gtk_button_new_from_icon_name("media-skip-forward", GTK_ICON_SIZE_SMALL_TOOLBAR);
    gtk_container_add(GTK_CONTAINER(controls), skip_forward);

    ap_gui->volume_adjustment = gtk_adjustment_new(0, 0, 101, 10, 1, 1);
    GtkWidget* volume_button = gtk_volume_button_new();
    gtk_container_add(GTK_CONTAINER(controls), volume_button);
    gtk_scale_button_set_adjustment((GtkScaleButton*)volume_button, ap_gui->volume_adjustment);

    gtk_widget_show_all(window);
}



int audioplayer_gui(int argc, char **argv) {
    AudioPlayerGUI ap_gui;
    ap_gui.ap = ap_init();

    GtkApplication* app = gtk_application_new("com.georgetian.audioplayer", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(activate), &ap_gui);
    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    ap_destroy(ap_gui.ap);

    return status;
}