int ivkid = 0;
static int SVCD_new_id(){
  int id = ivkid;
  ivkid = ivkid + 1;
  if (ivkid > 65535){
    ivkid = 0;
  }
  return ivkid;
}
