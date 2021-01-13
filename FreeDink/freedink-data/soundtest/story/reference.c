void talk(void)
{
  freeze(1);

  choice_start();
    "steps.wav"
    "sword1.wav"
    "Leave"
  choice_end();

  say("Playing...", 1);

  // steps.wav
  if (&result == 1)
  {
    // copy/pasted from S3-1ST.c
    playsound(40,22050,0,0,0);
    wait(1000);
    playsound(40,22050,0,0,0);
    wait(650);
    playsound(40,22050,0,0,0);
    wait(500);
  }

  // sword1.wav
  if (&result == 2)
  {
    // copy/pasted from S2-FIGHT.c
    playsound(36, 22050,0,&current_sprite, 0);
    // let's play it again
    wait(1000);
    playsound(36, 22050,0,&current_sprite, 0);
  }

  unfreeze(1);
}
