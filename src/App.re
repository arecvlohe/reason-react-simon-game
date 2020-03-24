type bgColor =
  | Green
  | Red
  | Blue
  | Yellow;

module Styles = {
  open Css;

  let container =
    style([
      display(`flex),
      justifyContent(`center),
      alignItems(`center),
      minHeight(`vh(100.0)),
    ]);

  let boxes =
    style([
      display(`flex),
      flexWrap(`wrap),
      maxWidth(`px(500)),
      maxHeight(`px(500)),
    ]);

  let box = (color: bgColor) => {
    let baseStyle = [minHeight(`px(250)), minWidth(`px(250))];
    let green = "07f767";
    let red = "f95e59";
    let blue = "00bcea";
    let yellow = "f4ed7c";
    let bgColor =
      switch (color) {
      | Green => backgroundColor(`hex(green))
      | Red => backgroundColor(`hex(red))
      | Blue => backgroundColor(`hex(blue))
      | Yellow => backgroundColor(`hex(yellow))
      };
    style([bgColor, ...baseStyle]);
  };
};

type sequence = array(int);

type state = {sequence};

type action =
  | SetSequence(sequence);

[@react.component]
let make = () => {
  let (_state, send) =
    ReactUpdate.useReducer({sequence: [||]}, (action, _state) =>
      switch (action) {
      | SetSequence(array) => Update({sequence: array})
      }
    );

  React.useEffect0(() => {
    let array =
      Belt.Array.makeBy(20, _i =>
        Js.Math.floor(Js.Math.random() *. 4.0 +. 1.0)
      );
    send(SetSequence(array));
    None;
  });

  <div className=Styles.container>
    <div className=Styles.boxes>
      <div
        className={Styles.box(Green)}
        onClick={_e => Sound.green->Audio.play}
      />
      <div
        className={Styles.box(Red)}
        onClick={_e => Sound.red->Audio.play}
      />
      <div
        className={Styles.box(Blue)}
        onClick={_e => Sound.blue->Audio.play}
      />
      <div
        className={Styles.box(Yellow)}
        onClick={_e => Sound.yellow->Audio.play}
      />
    </div>
  </div>;
};
