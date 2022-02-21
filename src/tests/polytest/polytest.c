/*
  polytest.c

  Test app for CRT X-Y library (libcrtxy)
  http://libcrtxy.sf.net/

  Bill Kendrick <bill@newbreedsoftware.com>

  August 13, 2008 - August 17, 2008
*/

#include <crtxy.h>

typedef struct point_s {
  XY_bool alive;
  XY_fixed x, y;
} point_t;

#define MAX_POINTS 100

point_t points[MAX_POINTS];


void add_point(XY_fixed x, XY_fixed y);
XY_bool handle_intersect(XY_lines * lines, XY_fixed x1, XY_fixed y1,
                         XY_fixed * x2, XY_fixed * y2);


int main(int argc, char * argv[])
{
  XY_options opts;
  XY_color black, white;
  XY_bool done, click;
  SDL_Event event;
  XY_lines * lines;
  XY_fixed new_line_x, new_line_y;
  int ret;
  int i, ticks, tick_speed;
  XY_fixed dist;

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

  if (!XY_init(&opts, 32<<XY_FIXED_SHIFT, 24<<XY_FIXED_SHIFT))
  {
    fprintf(stderr, "Error initializing libcrtxy: %s\n", XY_errstr());
    XY_print_options(stderr, opts);
    return(1);
  }

  black = XY_setcolor(0x00, 0x00, 0x00, 0xff);
  white = XY_setcolor(0xff, 0xff, 0xff, 0xff);

  XY_set_background(black, NULL, 0, 0, 0, 0);

  for (i = 0; i < MAX_POINTS; i++)
    points[i].alive = XY_FALSE;

  lines = XY_new_lines();

  new_line_x = 0;
  new_line_y = 0;
  click = XY_FALSE;
  ticks = 0;
  tick_speed = 1;

  done = XY_FALSE;

  do
  {
    XY_start_frame(10);

    for (i = 0; i < MAX_POINTS; i++)
      if (points[i].alive)
        XY_draw_point(points[i].x, points[i].y, white, XY_THIN);

    XY_draw_lines(lines);

    while (SDL_PollEvent(&event))
    {
      if (event.type == SDL_QUIT)
        done = XY_TRUE;
      else if (event.type == SDL_KEYDOWN)
      {
        if (event.key.keysym.sym == SDLK_ESCAPE)
          done = XY_TRUE;
      }
      else if (event.type == SDL_MOUSEBUTTONDOWN)
      {
        if (event.button.button == 1)
        {
          XY_start_lines(lines);
          new_line_x = XY_screenx_to_canvasx(event.button.x);
          new_line_y = XY_screenx_to_canvasx(event.button.y);
          click = XY_TRUE;
          tick_speed = 10;
        }
        else
        {
          add_point(event.button.x, event.button.y);
        }
      }
      else if (event.type == SDL_MOUSEBUTTONUP)
      {
        click = XY_FALSE;
        if (tick_speed < 1000)
          tick_speed = 1;
      }
      else if (event.type == SDL_MOUSEMOTION && click)
      {
        XY_fixed x1, y1, x2, y2;

        x1 = new_line_x;
        y1 = new_line_y;

        x2 = XY_screenx_to_canvasx(event.button.x);
        y2 = XY_screenx_to_canvasx(event.button.y);

        dist = XY_sqrt(XY_mult((x2 - x1), (x2 - x1)) +
                       XY_mult((y2 - y1), (y2 - y1)));

        if (dist > XY_FIXED_ONE)
        {
          if (handle_intersect(lines, x1, y1, &x2, &y2))
          {
            click = XY_FALSE;
            tick_speed = 1000;
            ticks = 1;
          }
        
          XY_add_line(lines, x1, y1, x2, y2, white, XY_THIN);

          new_line_x = x2;
          new_line_y = y2;
        }
      }
    }

    if (lines->count > 0)
    {
      if ((ticks % tick_speed) == 0)
      {
        lines->count--;
        if (lines->count > 0)
          memcpy(&(lines->lines[0]), &(lines->lines[1]),
                 sizeof(XY_line) * lines->count);
      }
    }
    
    ticks++;

    XY_end_frame(XY_TRUE);
  }
  while (!done);

  XY_quit();

  return(0);
}

XY_bool handle_intersect(XY_lines * lines, XY_fixed x1, XY_fixed y1,
                         XY_fixed * x2, XY_fixed * y2)
{
  XY_line l;
  int i;
  XY_fixed newx, newy;
  XY_intersection result;

  l.x1 = x1;
  l.y1 = y1;
  l.x2 = *x2;
  l.y2 = *y2;

  for (i = 0; i < lines->count; i++)
  {
    if ((lines->lines[i].x1 == l.x1 && lines->lines[i].y1 == l.y1) ||
        (lines->lines[i].x1 == l.x2 && lines->lines[i].y1 == l.y2) ||
        (lines->lines[i].x2 == l.x1 && lines->lines[i].y2 == l.y1) ||
        (lines->lines[i].x2 == l.x2 && lines->lines[i].y2 == l.y2))
    {
      /* Point A and Point B touch where they connect; that's ok */
    }
    else
    {
      if (XY_lines_intersect(lines->lines[i], l, &newx, &newy, &result))
      {
        if (result == XY_INTERSECTION_INTERSECTING)
        {
          *x2 = newx;
          *y2 = newy;

          lines->count -= i;
          memcpy(&(lines->lines[0]), &(lines->lines[i]),
                 sizeof(XY_line) * lines->count);

          lines->lines[0].x1 = *x2;
          lines->lines[0].y1 = *y2;

          return(XY_TRUE);
        }
      }
    }
  }

  return(XY_FALSE);
}

void add_point(XY_fixed x, XY_fixed y)
{
  int i, found;

  found = -1;

  for (i = 0; i < MAX_POINTS && found == -1; i++)
    if (points[i].alive == XY_FALSE)
      found = i;

  if (found == -1)
    found = rand() % MAX_POINTS;

  points[found].alive = XY_TRUE;
  points[found].x = XY_screenx_to_canvasx(x);
  points[found].y = XY_screenx_to_canvasx(y);
}
