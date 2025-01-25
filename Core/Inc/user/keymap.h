typedef enum _Key {
  Key1,
  Key2,
  Key3,
  KeyA,
  Key4,
  Key5,
  Key6,
  KeyB,
  Key7,
  Key8,
  Key9,
  KeyC,
  KeyStar,
  Key0,
  KeyNum,
  KeyD,
  KeyUp = Key2,
  KeyRight = Key6,
  KeyDown = Key8,
  KeyLeft = Key4,
  KeySelect = Key5,
  KeyBack = KeyD,
  KeyDel = KeyA,
} Key;

const char* KEY_SEQ[] = {
    "_,@1",  "abc2", "def3",  "", "ghi4", "jkl5", "mno6", "",
    "pqrs7", "tuv8", "wxyz9", "", " *",   "0",    "#",    "",
};

const Key KEYMAP[4][4] = {{Key1, Key2, Key3, KeyA},
                          {Key4, Key5, Key6, KeyB},
                          {Key7, Key8, Key9, KeyC},
                          {KeyStar, Key0, KeyNum, KeyD}};