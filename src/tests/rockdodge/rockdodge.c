/*
  rockdodge.c

  Simple game (test app for CRT X-Y library (libcrtxy))
  http://libcrtxy.sf.net/

  Bill Kendrick <bill@newbreedsoftware.com>

  August 3, 2008 - August 10, 2008
*/

#include <crtxy.h>

#define NUM_COLORS 5

#define WIDTH (32 << XY_FIXED_SHIFT)
#define HEIGHT (24 << XY_FIXED_SHIFT)

#define RADIUS XY_FIXED_HALF

#define NUM_ROCKS 10

typedef struct rock_s {
  XY_fixed x, y;
  XY_fixed xm, ym;
  int color;
  XY_fixed angle, anglem;
} rock_t;

void bounce(XY_fixed * a1, XY_fixed m1, XY_fixed * a2, XY_fixed m2);

int main(int argc, char * argv[])
{
  XY_options opts;
  XY_color black, white;
  XY_color colors[NUM_COLORS];
  XY_color flame_colors[3];
  XY_bool done;
  SDL_Event event;
  int i, ret, c;
  XY_fixed x, y, xm, ym;
  XY_fixed x1, y1, x2, y2, x3, y3, x4, y4, r;
  int angle;
  rock_t rocks[NUM_ROCKS];
  XY_bool key_ccw, key_cw, key_thrust;
  XY_bool throttle_fps;
  int ticks_since;
  int a;
  XY_lines * ship_lines;
  XY_lines * rock_lines;

  XY_default_options(&opts);

  ret = XY_load_options(&opts);
  if (!ret)
    return(1);

  ret = XY_parse_envvars(&opts);
  if (!ret)
    return(1);

  ret = XY_parse_options(&argc, argv, &opts);
  if (ret != 0)
  {
    fprintf(stderr, "Error setting libcrtxy options: %s\n", XY_errstr());
    fprintf(stderr, "Failed on %s\n", argv[ret]);
    return(1);
  }

  if (!XY_init(&opts, WIDTH, HEIGHT))
  {
    fprintf(stderr, "Error initializing libcrtxy: %s\n", XY_errstr());
    XY_print_options(stderr, opts);
    return(1);
  }

  ship_lines = XY_new_lines();
  if (ship_lines == NULL)
  {
    fprintf(stderr, "Error creating ship line collection: %s\n", XY_errstr());
    return(1);
  }

  rock_lines = XY_new_lines();
  if (rock_lines == NULL)
  {
    fprintf(stderr, "Error creating rock line collection: %s\n", XY_errstr());
    return(1);
  }

  srand(SDL_GetTicks());

  black = XY_setcolor(0x00, 0x00, 0x00, 0xff);
  white = XY_setcolor(0xff, 0xff, 0xff, 0xff);
  for (i = 0; i < NUM_COLORS; i++)
    colors[i] = XY_setcolor(128 + (rand() % 128),
                            128 + (rand() % 128),
                            128 + (rand() % 128),
                            0xff);

  flame_colors[0] = XY_setcolor(0xff, 0x00, 0x00, 0x88);
  flame_colors[1] = XY_setcolor(0xff, 0x88, 0x00, 0x88);
  flame_colors[2] = XY_setcolor(0xff, 0xff, 0x00, 0x88);


  XY_set_background(black, NULL, 0, 0, 0, 0);

  done = XY_FALSE;

  x = WIDTH / 2;
  y = HEIGHT / 2;
  xm = 0;
  ym = 0;
  angle = 90;

  key_ccw = XY_FALSE;
  key_cw = XY_FALSE;
  key_thrust = XY_FALSE;

  for (i = 0; i < NUM_ROCKS; i++)
  {
    rocks[i].x = rand() % WIDTH;
    rocks[i].y = rand() % HEIGHT;
    rocks[i].xm = ((rand() % XY_FIXED_ONE) - XY_FIXED_HALF) / 2;
    rocks[i].ym = ((rand() % XY_FIXED_ONE) - XY_FIXED_HALF) / 2;
    rocks[i].angle = rand() % (360 << XY_FIXED_SHIFT);
    rocks[i].anglem = (rand() % (XY_FIXED_ONE * 5)) - (XY_FIXED_HALF * 5);
    rocks[i].color = rand() % NUM_COLORS;
  }

  throttle_fps = XY_TRUE;
  ticks_since = 0;

  do
  {
    XY_start_frame(30);

    while (SDL_PollEvent(&event))
    {
      if (event.type == SDL_QUIT)
        done = XY_TRUE;
      else if (event.type == SDL_KEYDOWN)
      {
        if (event.key.keysym.sym == SDLK_ESCAPE)
          done = XY_TRUE;
        else if (event.key.keysym.sym == SDLK_UP)
          key_thrust = XY_TRUE;
        else if (event.key.keysym.sym == SDLK_LEFT)
          key_ccw = XY_TRUE;
        else if (event.key.keysym.sym == SDLK_RIGHT)
          key_cw = XY_TRUE;
        else if (event.key.keysym.sym == SDLK_f)
          throttle_fps = !throttle_fps;
      }
      else if (event.type == SDL_KEYUP)
      {
        if (event.key.keysym.sym == SDLK_UP)
          key_thrust = XY_FALSE;
        else if (event.key.keysym.sym == SDLK_LEFT)
          key_ccw = XY_FALSE;
        else if (event.key.keysym.sym == SDLK_RIGHT)
          key_cw = XY_FALSE;
      }
    }

    if (key_thrust)
    {
      xm += (((XY_cos(angle / 10) / 128) * ticks_since) / 33);
      ym -= (((XY_sin(angle / 10) / 128) * ticks_since) / 33);
    }

    if (key_cw)
    {
      angle -= ((150 * ticks_since) / 33);
      if (angle < 0)
        angle += 3600;
    }
    else if (key_ccw)
    {
      angle += ((150 * ticks_since) / 33);
      if (angle >= 3600)
        angle -= 3600;
    }

    x += ((xm * ticks_since) / 33);
    y += ((ym * ticks_since) / 33);
    if (x < 0)
      x += WIDTH;
    else if (x >= WIDTH)
      x -= WIDTH;
    if (y < 0)
      y += HEIGHT;
    else if (y >= HEIGHT)
      y -= HEIGHT;

    for (i = 0; i < NUM_ROCKS; i++)
    {
      rocks[i].x += ((rocks[i].xm * ticks_since) / 33);
      rocks[i].y += ((rocks[i].ym * ticks_since) / 33);
    
      if (rocks[i].x < 0)
        rocks[i].x += WIDTH;
      else if (rocks[i].x >= WIDTH)
        rocks[i].x -= WIDTH;
      if (rocks[i].y < 0)
        rocks[i].y += HEIGHT;
      else if (rocks[i].y >= HEIGHT)
        rocks[i].y -= HEIGHT;

      rocks[i].angle += ((rocks[i].anglem * ticks_since) / 33);
      if (rocks[i].angle < 0)
        rocks[i].angle += (360 << XY_FIXED_SHIFT);
      else if (rocks[i].angle >= (360 << XY_FIXED_SHIFT))
        rocks[i].angle -= (360 << XY_FIXED_SHIFT);
    }

    a = angle / 10;

    x1 = x + XY_mult(XY_cos(a - 135), RADIUS);
    y1 = y - XY_mult(XY_sin(a - 135), RADIUS);
    x2 = x + XY_mult(XY_cos(a), RADIUS * 2);
    y2 = y - XY_mult(XY_sin(a), RADIUS * 2);
    x3 = x + XY_mult(XY_cos(a + 135), RADIUS);
    y3 = y - XY_mult(XY_sin(a + 135), RADIUS);

    XY_start_lines(ship_lines);
    XY_add_line(ship_lines, x1, y1, x2, y2, white, XY_THIN);
    XY_add_line(ship_lines, x2, y2, x3, y3, white, XY_THIN);
    XY_add_line(ship_lines, x3, y3, x, y, white, XY_THIN);
    XY_add_line(ship_lines, x, y, x1, y1, white, XY_THIN);
    XY_draw_lines(ship_lines);

    if (key_thrust)
    {
      r = RADIUS + (rand() % RADIUS);
      x4 = x + XY_mult(XY_cos(a + 180), r);
      y4 = y - XY_mult(XY_sin(a + 180), r);

      c = rand() % 3;

      XY_draw_line(x1, y1, x4, y4, flame_colors[c], XY_THIN);
      XY_draw_line(x3, y3, x4, y4, flame_colors[c], XY_THIN);
    }

    if (key_cw)
    {
      x4 = x + XY_mult(XY_cos(a + 30), RADIUS * 2);
      y4 = y - XY_mult(XY_sin(a + 30), RADIUS * 2);

      c = rand() % 3;

      XY_draw_line(x2, y2, x4, y4, flame_colors[c], XY_THIN);
    }
    else if (key_ccw)
    {
      x4 = x + XY_mult(XY_cos(a - 30), RADIUS * 2);
      y4 = y - XY_mult(XY_sin(a - 30), RADIUS * 2);

      c = rand() % 3;

      XY_draw_line(x2, y2, x4, y4, flame_colors[c], XY_THIN);
    }

    for (i = 0; i < NUM_ROCKS; i++)
    {
      x1 = rocks[i].x +
        XY_mult(XY_cos((rocks[i].angle >> XY_FIXED_SHIFT)), RADIUS);
      y1 = rocks[i].y -
        XY_mult(XY_sin((rocks[i].angle >> XY_FIXED_SHIFT)), RADIUS);

      x2 = rocks[i].x +
        XY_mult(XY_cos((rocks[i].angle >> XY_FIXED_SHIFT) + 120), RADIUS);
      y2 = rocks[i].y -
        XY_mult(XY_sin((rocks[i].angle >> XY_FIXED_SHIFT) + 120), RADIUS);

      x3 = rocks[i].x +
        XY_mult(XY_cos((rocks[i].angle >> XY_FIXED_SHIFT) + 240), RADIUS);
      y3 = rocks[i].y -
        XY_mult(XY_sin((rocks[i].angle >> XY_FIXED_SHIFT) + 240), RADIUS);

      XY_start_lines(rock_lines);

      XY_add_line(rock_lines, x1, y1, x2, y2, colors[rocks[i].color], XY_THIN);
      XY_add_line(rock_lines, x2, y2, x3, y3, colors[rocks[i].color], XY_THIN);
      XY_add_line(rock_lines, x3, y3, x1, y1, colors[rocks[i].color], XY_THIN);

      XY_add_line(rock_lines, rocks[i].x, rocks[i].y, x1, y1,
                  colors[rocks[i].color], XY_THIN);
      XY_add_line(rock_lines, rocks[i].x, rocks[i].y, x2, y2,
                  colors[rocks[i].color], XY_THIN);
      XY_add_line(rock_lines, rocks[i].x, rocks[i].y, x3, y3,
                  colors[rocks[i].color], XY_THIN);

      XY_draw_lines(rock_lines);


      if (XY_line_groups_intersect(ship_lines, rock_lines))
      {
        /* Prevent them from getting stuck: */
        rocks[i].x -= rocks[i].xm;
        rocks[i].y -= rocks[i].ym;

        /* Ship's mass is 1.0, rocks are mass 0.5 */
        bounce(&xm, XY_FIXED_ONE, &(rocks[i].xm), XY_FIXED_HALF);
        bounce(&ym, XY_FIXED_ONE, &(rocks[i].ym), XY_FIXED_HALF);
      }
    }

    ticks_since = XY_end_frame(throttle_fps);
  }
  while (!done);

  XY_free_lines(ship_lines);
  XY_free_lines(rock_lines);
 
  XY_quit();

  return(0);
}

void bounce(XY_fixed * a1, XY_fixed m1, XY_fixed * a2, XY_fixed m2)
{
  XY_fixed v1, v2, new_v1, new_v2;

  v1 = *a1;
  v2 = *a2;

/*
  new_v1 = (v1 * (m1 - m2) + (2 * m2 * v2)) / (m1 + m2);
  new_v2 = (v2 * (m2 - m1) + (2 * m1 * v1)) / (m1 + m2);
*/

  new_v1 = XY_div((XY_mult(v1, (m1 - m2)) + XY_mult(2 * m2, v2)), (m1 + m2));
  new_v2 = XY_div((XY_mult(v2, (m2 - m1)) + XY_mult(2 * m1, v1)), (m1 + m2));

  *a1 = new_v1;
  *a2 = new_v2;
}
