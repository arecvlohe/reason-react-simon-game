class type audioInstance =
  [@bs]
  {
    pub play: unit => unit;
  };

type tAudio = Js.t(audioInstance);

[@bs.new] external createAudio: string => tAudio = "Audio";
