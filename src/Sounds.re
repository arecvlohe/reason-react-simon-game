open Types;

let green =
  Audio.createAudio("https://s3.amazonaws.com/freecodecamp/simonSound1.mp3");

let red =
  Audio.createAudio("https://s3.amazonaws.com/freecodecamp/simonSound2.mp3");

let blue =
  Audio.createAudio("https://s3.amazonaws.com/freecodecamp/simonSound3.mp3");

let yellow =
  Audio.createAudio("https://s3.amazonaws.com/freecodecamp/simonSound4.mp3");

let error =
  Audio.createAudio(
    "https://s3.amazonaws.com/adam-recvlohe-sounds/error.wav",
  );

let map = [(Green, green), (Red, red), (Blue, blue), (Yellow, yellow)];
