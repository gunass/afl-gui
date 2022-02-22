#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

/*
Licensed under the Gunass Public Licence (GaPL) version 0.1 which states:
	Do NOT use this code except for trivial educational purposes.
	This code comes with absolutely no warranty or guarantee.
	Shoot me an email if you have any questions.
*/

/*
This code is a partial GUI wrapper for:
	american fuzzy lop
  	Written and maintained by Michal Zalewski <lcamtuf@google.com>
	Forkserver design by Jann Horn <jannhorn@googlemail.com>	
written by Alex Epstein <aepstein@unimelb.edu.au>
*/


// Global struct holding each widget in the primary window
struct window_state {
  GtkFileChooserButton *binary_;
  GtkFileChooserButton *tests_;
  GtkFileChooserButton *output_;
  GtkFileChooserButton *dict_;

  GtkComboBoxText *determ_;
  GtkCheckButton *noninstr_;
  GtkSpinButton *memlimit_;
  GtkSpinButton *timelimit_;
  
  GtkEntry *extra_options_;
  GtkEntry *binary_args_;
};

struct window_state instance_state;


// Send the given command to afl-fuzz and retrieve via pipe
static void execute_afl(char* argv[]) {
	
	// Concatenate commands and intercalate with ' '
	char buf[1024];
	strcpy(buf, "./afl-fuzz ");
	for (int i = 1; argv[i]; i++) {
		strcpy(buf, strcat(buf, argv[i]));
		strcpy(buf, strcat(buf, " "));
	}
  	
  	// Call afl-fuzz and send its stdout to stream
  	char fbuf[1024];
	FILE *stream = popen(buf, "r");
	if (stream == NULL) {
		printf("Error with shell stream or pipe. Goodbye!");
		exit(1);
	}
	
	// Retrieve stream and print to terminal (this could be more efficient...)
	while (stream) {
		if (fgets(fbuf, 1024, stream)) {
			printf("%s", fbuf);
		}
	}
}
		
	
	
	

static void execute_state(GtkWidget *btn, gpointer data){
  
  // Buffers for button arguments
  char input_string[200];
  char output_string[200];
  char dict_string[200];
  char determ_string[5];
  char noninstr_string[5];
  char memlimit_string[9];
  char timelimit_string[9];
  char binary_string[200];
  
  char xopts_string[100];
  char bargs_string[100];

  char* argv[40];
  int i = 0;
  argv[i++] = "./afl-fuzz";
  
  // Get test cases folder from file chooser widget
  int input_length = sprintf(input_string, "%s", gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(instance_state.tests_)));
  if (!strcmp(input_string, "(null)")) {
      printf("No input specified!\n");
      return;
  }
  argv[i++] = "-i";
  argv[i++] = input_string;
  
  // Get output folder from file chooser 
  int output_length = sprintf(output_string, "%s", gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(instance_state.output_)));
  if (!strcmp(output_string, "(null)")) {
    printf("No output specified!\n");
    return;
  }
  argv[i++] = "-o";
  argv[i++] = output_string;
  
  // Get dictionary folder from file chooser
  int dict_length = sprintf(dict_string, "%s", gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(instance_state.dict_)));
  if (strcmp(dict_string, "(null)")) {
  	argv[i++] = "-x";
    argv[i++] = dict_string;
  }

  // Get determinism option from combo box ("all strategies", "nondeterminstic only", "deterministic only")
  char determ_option[40];
  sprintf(determ_option, "%s", gtk_combo_box_text_get_active_text(instance_state.determ_));

  if (!strcmp(determ_option, "Deterministic only")) {
    argv[i++] = "-D";
  } else if (!strcmp(determ_option, "Nondeterministic only")) {
    argv[i++] = "-d";
  }
  
  // Get instrumentation status from toggle button (i.e. QEMU mode)
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(instance_state.noninstr_))) {
    argv[i++] = "-Q";
  }

  // Get desired memory/timeout limits
  sprintf(memlimit_string,"%d",gtk_spin_button_get_value_as_int(instance_state.memlimit_));
  argv[i++] = "-m";
  argv[i++] = memlimit_string;
  sprintf(timelimit_string,"%d",gtk_spin_button_get_value_as_int(instance_state.timelimit_));
  argv[i++] = "-t";
  argv[i++] = timelimit_string;
  
  // Get custom afl-fuzz options
  sprintf(xopts_string, "%s", gtk_entry_get_text(GTK_ENTRY(instance_state.extra_options_)));
  int wordstart = 1;
  for (int j = 0; xopts_string[j]; j++) {
  	if (wordstart && xopts_string[j] != ' ') {
  		argv[i++] = &xopts_string[j];
  		wordstart = 0;
  	} else if (xopts_string[j] == ' ') {
  		xopts_string[j] = '\0';
  		wordstart = 1;
  	}
  }
  
  // Get binary file from file chooser (no file type checking... rely on afl-fuzz)
  int binary_length = sprintf(binary_string, "%s", gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(instance_state.binary_)));
  if (!strcmp(binary_string, "(null)")) {
    printf("Binary not specified!\n");
    return;
  }
  argv[i++] = binary_string;
  
  // Get binary arguments to append 
  sprintf(bargs_string, "%s", gtk_entry_get_text(GTK_ENTRY(instance_state.binary_args_)));
  wordstart = 1;
  for (int j = 0; bargs_string[j]; j++) {
  	if (wordstart && bargs_string[j] != ' ') {
  		argv[i++] = &bargs_string[j];
  		wordstart = 0;
  	} else if (bargs_string[j] == ' ') {
  		bargs_string[j] = '\0';
  		wordstart = 1;
  	}
  }
  
  // Null terminate to be safe
  argv[i] = 0;

  // Print arguments (debug)
  for (int j = 0; argv[j]; j++) {
    printf("%s\n", argv[j]);
  }
  printf("\n");
  
  // Send it away!
  execute_afl(argv);

}


static void on_app_activate(GApplication *app, gpointer data) {

  // Declare everything in the window
  GtkWindow *mainwindow;
  GtkWidget *icon;
  GtkBox *box0;
  GtkListBox *box1;

  GtkFileChooserButton *binary;
  GtkFileChooserButton *tests;
  GtkFileChooserButton *output;
  GtkFileChooserButton *dict;
  
  GtkComboBoxText *determ;
  GtkCheckButton *noninstr;
  GtkSpinButton *memlimit;
  GtkSpinButton *timelimit;
  GtkButton *execute;
  GtkButton *open_pfl;
  GtkButton *save_pfl;
  
  GtkEntry *extra_options;
  GtkEntry *binary_args;

  // Define boxes/views
  mainwindow = GTK_WINDOW(gtk_application_window_new(GTK_APPLICATION(app)));
  icon       = gtk_image_new_from_file("icon.png");
  box0       = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL,0));
  box1       = GTK_LIST_BOX(gtk_list_box_new());

  // Define file choosers
  binary     = GTK_FILE_CHOOSER_BUTTON(gtk_file_chooser_button_new ("Binary", GTK_FILE_CHOOSER_ACTION_OPEN));
  tests      = GTK_FILE_CHOOSER_BUTTON(gtk_file_chooser_button_new ("Test cases folder", GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER));
  output     = GTK_FILE_CHOOSER_BUTTON(gtk_file_chooser_button_new ("Output folder", GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER));
  dict       = GTK_FILE_CHOOSER_BUTTON(gtk_file_chooser_button_new ("Dictionary (optional)", GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER));
  
  //Define buttons
  determ     = GTK_COMBO_BOX_TEXT(gtk_combo_box_text_new());
  noninstr   = GTK_CHECK_BUTTON(gtk_check_button_new_with_label("Non-instrumented binary"));
  memlimit   = GTK_SPIN_BUTTON(gtk_spin_button_new_with_range(100,8000,100));
  timelimit  = GTK_SPIN_BUTTON(gtk_spin_button_new_with_range(100,8000,100));
  execute    = GTK_BUTTON(gtk_button_new_with_label("Go!"));
  open_pfl   = GTK_BUTTON(gtk_button_new_with_label("Open profile..."));
  save_pfl   = GTK_BUTTON(gtk_button_new_with_label("Save profile..."));
  
  //Define text entries
  extra_options = GTK_ENTRY(gtk_entry_new());
  binary_args   = GTK_ENTRY(gtk_entry_new());
  
  gtk_entry_set_placeholder_text(extra_options, "Extra options");
  gtk_entry_set_placeholder_text(binary_args, "Binary arguments");

  //Set up window
  gtk_window_set_title(GTK_WINDOW(mainwindow), "AFL-GUI");
  gtk_window_set_default_size(GTK_WINDOW(mainwindow),200,400);
  gtk_window_set_resizable(GTK_WINDOW(mainwindow), false);

  // Set up spin buttons to be a little more convenient
  gtk_spin_button_set_value(memlimit, 1000);
  gtk_spin_button_set_value(timelimit, 1000);
  
  GtkBox *membox = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0));
  GtkBox *timebox = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0));
  
  gtk_container_add(GTK_CONTAINER(membox), GTK_WIDGET(memlimit));
  gtk_container_add(GTK_CONTAINER(membox), GTK_WIDGET(gtk_label_new("Memory (MB)")));
  
  gtk_container_add(GTK_CONTAINER(timebox), GTK_WIDGET(timelimit));
  gtk_container_add(GTK_CONTAINER(timebox), GTK_WIDGET(gtk_label_new("Timeout (ms)")));

  // Set up file chooser grid
  GtkGrid *chooser_grid = GTK_GRID(gtk_grid_new());
  
  GtkLabel *binary_label = GTK_LABEL(gtk_label_new("  Binary:"));
  GtkLabel *tests_label  = GTK_LABEL(gtk_label_new("  Tests:"));
  GtkLabel *output_label = GTK_LABEL(gtk_label_new("  Output:"));
  GtkLabel *dict_label   = GTK_LABEL(gtk_label_new("  Dict:"));

  gtk_widget_set_halign(GTK_WIDGET(binary_label),GTK_ALIGN_START);
  gtk_widget_set_halign(GTK_WIDGET(tests_label),GTK_ALIGN_START);
  gtk_widget_set_halign(GTK_WIDGET(output_label),GTK_ALIGN_START);
  gtk_widget_set_halign(GTK_WIDGET(dict_label),GTK_ALIGN_START);

  gtk_grid_attach(chooser_grid, GTK_WIDGET(binary_label),0,0,1,1);
  gtk_grid_attach(chooser_grid, GTK_WIDGET(binary),1,0,1,1);
  gtk_grid_attach(chooser_grid, GTK_WIDGET(tests_label),0,1,1,1);
  gtk_grid_attach(chooser_grid, GTK_WIDGET(tests),1,1,1,1);
  gtk_grid_attach(chooser_grid, GTK_WIDGET(output_label),0,2,1,1);
  gtk_grid_attach(chooser_grid, GTK_WIDGET(output),1,2,1,1);
  gtk_grid_attach(chooser_grid, GTK_WIDGET(dict_label),0,3,1,1);
  gtk_grid_attach(chooser_grid, GTK_WIDGET(dict),1,3,1,1);

  gtk_grid_set_column_spacing(chooser_grid, (guint) 10);

  //Set up deterministic combo box
  gtk_combo_box_text_append(determ, "dnd", "All strategies");
  gtk_combo_box_text_append(determ, "d", "Deterministic only");
  gtk_combo_box_text_append(determ, "nd", "Nondeterministic only");
  gtk_combo_box_set_active_id(GTK_COMBO_BOX(determ),"dnd");

  //Set up options box

  gtk_container_add(GTK_CONTAINER(box1), GTK_WIDGET(determ));
  gtk_container_add(GTK_CONTAINER(box1), GTK_WIDGET(noninstr));
  gtk_container_add(GTK_CONTAINER(box1), GTK_WIDGET(membox));
  gtk_container_add(GTK_CONTAINER(box1), GTK_WIDGET(timebox));
  gtk_container_add(GTK_CONTAINER(box1), GTK_WIDGET(extra_options));
  gtk_container_add(GTK_CONTAINER(box1), GTK_WIDGET(binary_args));
  
  // Compose box0 - the main box
  gtk_container_add(GTK_CONTAINER(box0), GTK_WIDGET(icon));
  gtk_container_add(GTK_CONTAINER(box0), GTK_WIDGET(chooser_grid));
  gtk_container_add(GTK_CONTAINER(box0), GTK_WIDGET(box1));
  gtk_container_add(GTK_CONTAINER(box0), GTK_WIDGET(execute));
  gtk_container_add(GTK_CONTAINER(box0), GTK_WIDGET(gtk_separator_new(GTK_ORIENTATION_HORIZONTAL)));
  gtk_container_add(GTK_CONTAINER(box0), GTK_WIDGET(open_pfl));
  gtk_container_add(GTK_CONTAINER(box0), GTK_WIDGET(save_pfl));

  // Add all interactable widgets to a nice clean struct
  instance_state.binary_ = binary;
  instance_state.tests_  = tests;
  instance_state.output_ = output;
  instance_state.dict_   = dict;

  instance_state.determ_ = determ;
  instance_state.noninstr_ = noninstr;
  instance_state.memlimit_ = memlimit;
  instance_state.timelimit_ = timelimit;
  
  instance_state.extra_options_ = extra_options;
  instance_state.binary_args_ = binary_args;

  // Get the signal for GO
  g_signal_connect(execute, "clicked", G_CALLBACK(execute_state), NULL);
  
  // Add main box to window and show
  gtk_container_add(GTK_CONTAINER(mainwindow), GTK_WIDGET(box0));

  gtk_widget_show_all(GTK_WIDGET(mainwindow));
  gtk_window_present(GTK_WINDOW(mainwindow));

}
  
int main(int argc, char *argv[]) {
  GtkApplication *app = gtk_application_new(
					    "com.github.gunass.afl_gui",
					    G_APPLICATION_FLAGS_NONE
					    );

  g_signal_connect(app, "activate", G_CALLBACK(on_app_activate), NULL);
  int status = g_application_run(G_APPLICATION(app), argc, argv);

  g_object_unref(app);
  return status;
}
