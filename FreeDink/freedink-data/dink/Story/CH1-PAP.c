//script for chest with gold heart in it

void main( void)
{
preload_seq(175);
}

void hit ( void )
 {
  //play noise
  int &hold = sp_editor_num(&current_sprite);

  if (&hold != 0)
    {
     //this was placed by the editor, lets make the chest stay open
     editor_type(&hold, 4); 
     editor_seq(&hold, 175);
     editor_frame(&hold, 4);
     //type means show this seq/frame combo as background in the future
    }
  &save_x = sp_x(&current_sprite, -1);
  &save_y = sp_y(&current_sprite, -1);
  external("make", "papgold");

  sp_seq(&current_sprite, 175);
  sp_script(&current_sprite, "");
  sp_notouch(&current_sprite, 1);
  sp_nohit(&current_sprite, 1);
  kill_this_task();
 }
