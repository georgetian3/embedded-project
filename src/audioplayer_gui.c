#include <gtk/gtk.h>
#include "audioplayer.h"

static AudioPlayer* ap;    
static GtkWidget* playlist;
static GtkAdjustment* slider_adjustment;
static GtkAdjustment* volume_adjustment;
static GtkWidget* timestamp_label;
static GtkWidget* info_label;
static bool stop_moving_slider;
static bool waiting_for_scan;
static bool scan_complete;

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

static void set_info(const char* text) {
    gtk_label_set_text((GtkLabel*)info_label, text);
}

static void file_clicked(GtkWidget* widget, gpointer data) {
    int ret = ap_open(ap, ap_get_audio_filenames(ap)[(int)data]);
    if (ret) {
        set_info(ap_errors[ret]);
    } else {
        char info[1024];
        sprintf(info, "File opened successfully: %s", ap_get_audio_filenames(ap)[(int)data]);
        set_info(info);
    }
}

static void skip_backward_clicked(GtkWidget* widget, gpointer data) {
    g_print("skip_backward_clicked\n");
}
static void skip_forward_clicked(GtkWidget* widget, gpointer data) {
}
static void seek_backward_clicked(GtkWidget* widget, gpointer data) {
    ap_set_timestamp(ap, ap_get_timestamp(ap) - 10);
}
static void seek_forward_clicked(GtkWidget* widget, gpointer data) {
    ap_set_timestamp(ap, ap_get_timestamp(ap) + 10);
}
static void repeat_clicked(GtkWidget* widget, gpointer data) {
    ap_set_repeat(ap, !ap_get_repeat(ap));
    gtk_button_set_icon(widget, ap_get_repeat(ap) ? "media-playlist-consecutive" : "media-playlist-repeat", GTK_ICON_SIZE_BUTTON);
}

static void play_pause_clicked(GtkWidget* widget, gpointer data) {
    g_print("Play/pause clicked\n");
    error_print(ap_play_pause(ap));
    g_print("is_playing: %d\n", ap_is_playing(ap));
    gtk_button_set_icon(widget, ap_is_playing(ap) ? "media-playback-pause" : "media-playback-start", GTK_ICON_SIZE_BUTTON);
}



static void set_slider(double value) {
    if (stop_moving_slider) {
        return;
    }
    gtk_adjustment_set_value(slider_adjustment, value);
}
static void volume_changed(GtkWidget* widget, gpointer data) {
    ap_set_volume(ap, gtk_adjustment_get_value(volume_adjustment));
}


static void set_timestamp_text(GtkWidget* widget, gpointer data) {
    Duration timestamp = seconds_to_duration(ap_get_timestamp(ap));
    Duration duration = seconds_to_duration(ap_duration(ap));
    char timestamp_buffer[] = "00:00:00/00:00:00";
    if (timestamp.seconds != -1) {
        sprintf(timestamp_buffer, "%02d:%02d:%02d/%02d:%02d:%02d", 
            timestamp.hours, timestamp.minutes, timestamp.seconds, 
            duration.hours, duration.minutes, duration.seconds
        );
    }
    gtk_label_set_text((GtkLabel*)timestamp_label, timestamp_buffer);
}

static void slider_pressed(GtkWidget* widget, gpointer data) {
    g_print("Slider pressed\n");
    stop_moving_slider = true;
    //set_timestamp_text(NULL, NULL);
}

static void slider_released(GtkWidget* widget, gpointer data) {
    double slider_value = (double)gtk_adjustment_get_value(slider_adjustment);
    g_print("Slider released: %f\n", slider_value);
    //ap_set_timestamp(ap, slider_value);
    //set_timestamp_text(NULL, NULL);
    stop_moving_slider = false;
}

static gboolean second_timeout(gpointer data) {
    set_timestamp_text(NULL, NULL);
    set_slider(ap_get_timestamp(ap));
    return G_SOURCE_CONTINUE;
}

static void activate(GtkApplication* app, gpointer data) {


    GtkWidget* window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Audio Player");
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);

    GtkWidget* vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    GtkWidget* playlist_scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(vbox), playlist_scroll);
    gtk_widget_set_vexpand(playlist_scroll, TRUE);

    playlist = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
    gtk_container_add(GTK_CONTAINER(playlist_scroll), playlist);
    gtk_scrolled_window_set_min_content_height((GtkScrolledWindow*)playlist_scroll, 200);

    ap_scan_dir(ap, "./");
    int audio_file_count = ap_audio_file_count(ap);
    const char** audio_filenames = ap_get_audio_filenames(ap);
    for (int i = 0; i < audio_file_count; i++) {
        GtkWidget* button = gtk_button_new_with_label(audio_filenames[i]);
        gtk_container_add(GTK_CONTAINER(playlist), button);
        g_signal_connect(button, "clicked", G_CALLBACK(file_clicked), (void*)i);
    }

    info_label = gtk_label_new("Click an audio file to open");
    gtk_container_add(GTK_CONTAINER(vbox), info_label);

    GtkWidget* slider_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
    gtk_container_add(GTK_CONTAINER(vbox), slider_box);

    slider_adjustment = gtk_adjustment_new(0, 0, 100, 10, 0, 0);
    GtkWidget* slider = gtk_scale_new(GTK_ORIENTATION_HORIZONTAL, slider_adjustment);
    gtk_container_add(GTK_CONTAINER(slider_box), slider);
    gtk_scale_set_draw_value((GtkScale*)slider, FALSE);
    gtk_widget_set_hexpand(slider, TRUE);
    g_signal_connect(slider, "button-press-event", G_CALLBACK(slider_pressed), NULL);
    g_signal_connect(slider, "button-release-event", G_CALLBACK(slider_released), NULL);

    timestamp_label = gtk_label_new("00:00:00/00:00:00");
    gtk_container_add(GTK_CONTAINER(slider_box), timestamp_label);

    GtkWidget* controls = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
    gtk_widget_set_halign(controls, GTK_ALIGN_CENTER);
    gtk_container_add(GTK_CONTAINER(vbox), controls);

    GtkWidget* repeat = gtk_button_new_from_icon_name("media-playlist-repeat", GTK_ICON_SIZE_BUTTON);
    gtk_container_add(GTK_CONTAINER(controls), repeat);
    g_signal_connect(repeat, "clicked", G_CALLBACK(repeat_clicked), NULL);

    GtkWidget* skip_backward = gtk_button_new_from_icon_name("media-skip-backward", GTK_ICON_SIZE_BUTTON);
    gtk_container_add(GTK_CONTAINER(controls), skip_backward);
    g_signal_connect(skip_backward, "clicked", G_CALLBACK(skip_backward_clicked), NULL);

    GtkWidget* seek_backward = gtk_button_new_from_icon_name("media-seek-backward", GTK_ICON_SIZE_BUTTON);
    gtk_container_add(GTK_CONTAINER(controls), seek_backward);
    g_signal_connect(seek_backward, "clicked", G_CALLBACK(seek_backward_clicked), NULL);

    GtkWidget* play_pause = gtk_button_new_from_icon_name("media-playback-start", GTK_ICON_SIZE_BUTTON);
    gtk_container_add(GTK_CONTAINER(controls), play_pause);
    g_signal_connect(play_pause, "clicked", G_CALLBACK(play_pause_clicked), NULL);

    GtkWidget* seek_forward = gtk_button_new_from_icon_name("media-seek-forward", GTK_ICON_SIZE_BUTTON);
    gtk_container_add(GTK_CONTAINER(controls), seek_forward);
    g_signal_connect(seek_forward, "clicked", G_CALLBACK(seek_forward_clicked), NULL);

    GtkWidget* skip_forward = gtk_button_new_from_icon_name("media-skip-forward", GTK_ICON_SIZE_SMALL_TOOLBAR);
    gtk_container_add(GTK_CONTAINER(controls), skip_forward);
    g_signal_connect(skip_forward, "clicked", G_CALLBACK(skip_forward_clicked), NULL);

    volume_adjustment = gtk_adjustment_new(ap_get_volume(ap), 0, 101, 10, 1, 1);
    GtkWidget* volume_button = gtk_volume_button_new();
    gtk_container_add(GTK_CONTAINER(controls), volume_button);
    gtk_scale_button_set_adjustment((GtkScaleButton*)volume_button, volume_adjustment);
    g_signal_connect(volume_adjustment, "value-changed", G_CALLBACK(volume_changed), NULL);


    g_timeout_add_seconds(1, second_timeout, NULL);

    gtk_widget_show_all(window);

}



int audioplayer_gui(int argc, char **argv) {
    ap = ap_init();

    GtkApplication* app = gtk_application_new("com.georgetian.audioplayer", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    ap_destroy(ap);

    return status;
}