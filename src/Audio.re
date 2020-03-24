type t;
[@bs.new] external make: string => t = "Audio";
[@bs.send] external play: t => unit = "play";
