void main( void )
{
preload_seq(452);
sp_touch_damage(&current_sprite, -1);

}


void touch( void )
{
freeze(1);
sp_x(1, 283);
sp_y(1, 320);
sp_seq(1, 452);
sp_frame(1, 1);
sp_nocontrol(1, 1); //dink can't move until anim is done!
sp_touch_damage(&current_sprite, 0);
sp_brain(1, 0);
wait(2000);
script_attach(1000);
fade_down();
&player_map = 198;
sp_x(1, 315);
sp_y(1, 375);
load_screen(198);
sp_brain(1, 1);
draw_screen();
fade_up();
kill_this_task();
}
