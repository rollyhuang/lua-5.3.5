#include "nuklear.h"
#include "nuklear_internal.h"

/* ==============================================================
 *
 *                          COLOR PICKER
 *
 * ===============================================================*/
NK_LIB nk_bool
nk_color_picker_behavior(nk_flags* state,
                         const nk_rect* bounds, const nk_rect* matrix,
                         const nk_rect* hue_bar, const nk_rect* alpha_bar,
                         nk_colorf* color, const nk_input* in) {
  float hsva[4];
  nk_bool value_changed = 0;
  nk_bool hsv_changed = 0;

  NK_ASSERT(state);
  NK_ASSERT(matrix);
  NK_ASSERT(hue_bar);
  NK_ASSERT(color);

  /* color matrix */
  nk_colorf_hsva_fv(hsva, *color);
  if (nk_do_button_behavior(state, *matrix, in, NK_BUTTON_REPEATER)) {
    hsva[1] = NK_SATURATE((in->mouse.pos.x - matrix->x) / (matrix->w - 1));
    hsva[2] = 1.0f - NK_SATURATE((in->mouse.pos.y - matrix->y) / (matrix->h - 1));
    value_changed = hsv_changed = 1;
  }
  /* hue bar */
  if (nk_do_button_behavior(state, *hue_bar, in, NK_BUTTON_REPEATER)) {
    hsva[0] = NK_SATURATE((in->mouse.pos.y - hue_bar->y) / (hue_bar->h - 1));
    value_changed = hsv_changed = 1;
  }
  /* alpha bar */
  if (alpha_bar) {
    if (nk_do_button_behavior(state, *alpha_bar, in, NK_BUTTON_REPEATER)) {
      hsva[3] = 1.0f - NK_SATURATE((in->mouse.pos.y - alpha_bar->y) / (alpha_bar->h - 1));
      value_changed = 1;
    }
  }
  nk_widget_state_reset(state);
  if (hsv_changed) {
    *color = nk_hsva_colorfv(hsva);
    *state = NK_WIDGET_STATE_ACTIVE;
  }
  if (value_changed) {
    color->a = hsva[3];
    *state = NK_WIDGET_STATE_ACTIVE;
  }
  /* set color picker widget state */
  if (nk_input_is_mouse_hovering_rect(in, *bounds))
    *state = NK_WIDGET_STATE_HOVERED;
  if (*state & NK_WIDGET_STATE_HOVER && !nk_input_is_mouse_prev_hovering_rect(in, *bounds))
    *state |= NK_WIDGET_STATE_ENTERED;
  else if (nk_input_is_mouse_prev_hovering_rect(in, *bounds))
    *state |= NK_WIDGET_STATE_LEFT;
  return value_changed;
}
NK_LIB void nk_draw_color_picker(nk_command_buffer* o, const nk_rect* matrix,
                                 const nk_rect* hue_bar, const nk_rect* alpha_bar,
                                 nk_colorf col) {
  NK_STORAGE const nk_color black = {0, 0, 0, 255};
  NK_STORAGE const nk_color white = {255, 255, 255, 255};
  NK_STORAGE const nk_color black_trans = {0, 0, 0, 0};

  const float crosshair_size = 7.0f;
  nk_color temp;
  float hsva[4];
  float line_y;
  int i;

  NK_ASSERT(o);
  NK_ASSERT(matrix);
  NK_ASSERT(hue_bar);

  /* draw hue bar */
  nk_colorf_hsva_fv(hsva, col);
  for (i = 0; i < 6; ++i) {
    NK_GLOBAL const nk_color hue_colors[] = {
        {255, 0, 0, 255}, {255, 255, 0, 255}, {0, 255, 0, 255}, {0, 255, 255, 255}, {0, 0, 255, 255}, {255, 0, 255, 255}, {255, 0, 0, 255}};
    nk_fill_rect_multi_color(o,
                             nk_make_rect(hue_bar->x, hue_bar->y + (float)i * (hue_bar->h / 6.0f) + 0.5f, hue_bar->w, (hue_bar->h / 6.0f) + 0.5f),
                             hue_colors[i],
                             hue_colors[i],
                             hue_colors[i + 1],
                             hue_colors[i + 1]);
  }
  line_y = (float)(int)(hue_bar->y + hsva[0] * matrix->h + 0.5f);
  nk_stroke_line(o, hue_bar->x - 1, line_y, hue_bar->x + hue_bar->w + 2, line_y, 1, nk_rgb(255, 255, 255));

  /* draw alpha bar */
  if (alpha_bar) {
    float alpha = NK_SATURATE(col.a);
    line_y = (float)(int)(alpha_bar->y + (1.0f - alpha) * matrix->h + 0.5f);

    nk_fill_rect_multi_color(o, *alpha_bar, white, white, black, black);
    nk_stroke_line(o, alpha_bar->x - 1, line_y, alpha_bar->x + alpha_bar->w + 2, line_y, 1, nk_rgb(255, 255, 255));
  }

  /* draw color matrix */
  temp = nk_hsv_f(hsva[0], 1.0f, 1.0f);
  nk_fill_rect_multi_color(o, *matrix, white, temp, temp, white);
  nk_fill_rect_multi_color(o, *matrix, black_trans, black_trans, black, black);

  /* draw cross-hair */
  {
    nk_vec2 p;
    float S = hsva[1];
    float V = hsva[2];
    p.x = (float)(int)(matrix->x + S * matrix->w);
    p.y = (float)(int)(matrix->y + (1.0f - V) * matrix->h);
    nk_stroke_line(o, p.x - crosshair_size, p.y, p.x - 2, p.y, 1.0f, white);
    nk_stroke_line(o, p.x + crosshair_size + 1, p.y, p.x + 3, p.y, 1.0f, white);
    nk_stroke_line(o, p.x, p.y + crosshair_size + 1, p.x, p.y + 3, 1.0f, white);
    nk_stroke_line(o, p.x, p.y - crosshair_size, p.x, p.y - 2, 1.0f, white);
  }
}
NK_LIB nk_bool
nk_do_color_picker(nk_flags* state,
                   nk_command_buffer* out, nk_colorf* col,
                   nk_color_format fmt, nk_rect bounds,
                   nk_vec2 padding, const nk_input* in,
                   const nk_user_font* font) {
  int ret = 0;
  nk_rect matrix;
  nk_rect hue_bar;
  nk_rect alpha_bar;
  float bar_w;

  NK_ASSERT(out);
  NK_ASSERT(col);
  NK_ASSERT(state);
  NK_ASSERT(font);
  if (!out || !col || !state || !font)
    return ret;

  bar_w = font->height;
  bounds.x += padding.x;
  bounds.y += padding.x;
  bounds.w -= 2 * padding.x;
  bounds.h -= 2 * padding.y;

  matrix.x = bounds.x;
  matrix.y = bounds.y;
  matrix.h = bounds.h;
  matrix.w = bounds.w - (3 * padding.x + 2 * bar_w);

  hue_bar.w = bar_w;
  hue_bar.y = bounds.y;
  hue_bar.h = matrix.h;
  hue_bar.x = matrix.x + matrix.w + padding.x;

  alpha_bar.x = hue_bar.x + hue_bar.w + padding.x;
  alpha_bar.y = bounds.y;
  alpha_bar.w = bar_w;
  alpha_bar.h = matrix.h;

  ret = nk_color_picker_behavior(state, &bounds, &matrix, &hue_bar, (fmt == NK_RGBA) ? &alpha_bar : 0, col, in);
  nk_draw_color_picker(out, &matrix, &hue_bar, (fmt == NK_RGBA) ? &alpha_bar : 0, *col);
  return ret;
}
NK_API nk_bool
nk_color_pick(nk_context* ctx, nk_colorf* color,
              nk_color_format fmt) {
  nk_window* win;
  nk_panel* layout;
  const nk_style* config;
  const nk_input* in;

  nk_widget_layout_states state;
  nk_rect bounds;

  NK_ASSERT(ctx);
  NK_ASSERT(color);
  NK_ASSERT(ctx->current);
  NK_ASSERT(ctx->current->layout);
  if (!ctx || !ctx->current || !ctx->current->layout || !color)
    return 0;

  win = ctx->current;
  config = &ctx->style;
  layout = win->layout;
  state = nk_widget(&bounds, ctx);
  if (!state)
    return 0;
  in = (state == NK_WIDGET_ROM || layout->flags & NK_WINDOW_ROM) ? 0 : &ctx->input;
  return nk_do_color_picker(&ctx->last_widget_state, &win->buffer, color, fmt, bounds, nk_make_vec2(0, 0), in, config->font);
}
NK_API nk_colorf
nk_color_picker(nk_context* ctx, nk_colorf color,
                nk_color_format fmt) {
  nk_color_pick(ctx, &color, fmt);
  return color;
}
