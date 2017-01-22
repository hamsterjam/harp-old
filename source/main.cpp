#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>

#include <globals.h>

#include <iostream>

using namespace std;

void allegroInit();
void allegroCleanup();

/*
 * Game Stuff Start
 */

void init() {
}

void update(double deltaT) {
}

void draw() {
}

void cleanup() {
}

/*
 * Game Stuff End
 */

int main(int argc, char** argv) {
   // Initialise
   disp = 0;
   timer = 0;
   eq = 0;

   allegroInit();
   init();

   // Game loop
   bool exit = false;
   bool needDraw = false;
   bool needUpdate = false;
   double prevTime = 0;

   al_start_timer(timer);
   while(!exit) {

      ALLEGRO_EVENT e;
      al_wait_for_event(eq, &e);

      if (e.type == ALLEGRO_EVENT_TIMER) {
         needDraw = true;
      }
      else if (e.type == ALLEGRO_EVENT_KEY_DOWN) {
         if (e.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
            exit = true;
         }
      }
      else if (e.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
         exit = true;
      }

      // Waiting for empty queue ensures keyboard / mouse state are
      // up to date and don't lag behind actual, physical state
      if (needUpdate && al_is_event_queue_empty(eq)) {
         needUpdate = false;

         double currTime = al_get_time();
         // Skip the first frame. It's ok, trust me
         if (prevTime != 0) {
            double deltaT = currTime - prevTime;
            update(deltaT);
         }
         prevTime = currTime;
      }

      if (needDraw && al_is_event_queue_empty(eq)) {
         needDraw = false;

         al_set_target_backbuffer(disp);
         al_clear_to_color(al_map_rgb(255, 255, 255));

         draw();

         al_flip_display();
      }
   }
   allegroCleanup();
   cleanup();

   return 0;
}

void allegroInit() {
   if (!al_init()) {
      cerr << "Failed to initialise Allegro." << endl;
      exit(1);
   }
   // Image Addon
   if (!al_init_image_addon()) {
      cerr << "Failed to initialise Allegro image addon." << endl;
      exit(1);
   }
   // Primitives Addon
   if (!al_init_primitives_addon()) {
       cerr << "Failed to initialsie Allegro primitives addon." << endl;
       exit(1);
   }
   if (!al_install_keyboard()) {
      cerr << "Failed to install keyboard." << endl;
      exit(1);
   }
   if (!al_install_mouse()) {
      cerr << "Failed to install mouse." << endl;
      exit(1);
   }

   timer = al_create_timer(FRAME_TIME);
   if (!timer) {
      cerr << "Failed to create timer." << endl;
      allegroCleanup();
      exit(1);
   }

   disp = al_create_display(SCREEN_WIDTH, SCREEN_HEIGHT);
   if (!disp) {
      cerr << "Failed to create display." << endl;
      allegroCleanup();
      exit(1);
   }

   eq = al_create_event_queue();
   if (!eq) {
      cerr << "Failed to create event queue." << endl;
      allegroCleanup();
      exit(1);
   }

   al_register_event_source(eq, al_get_keyboard_event_source());
   al_register_event_source(eq, al_get_mouse_event_source());
   al_register_event_source(eq, al_get_timer_event_source(timer));
   al_register_event_source(eq, al_get_display_event_source(disp));
}

void allegroCleanup() {
   if (disp) al_destroy_display(disp);
   if (timer) al_destroy_timer(timer);
   if (eq) al_destroy_event_queue(eq);
}
