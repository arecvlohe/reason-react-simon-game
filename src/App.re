type sequence = array(int);

type state = {sequence};

type action =
  | SetSequence(sequence);

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
  let box = (color: Types.colors) => {
    let baseStyle = [minHeight(`px(250)), minWidth(`px(250))];

    let bgColor =
      switch (color) {
      | Green => backgroundColor(`hex("07f767"))
      | Red => backgroundColor(`hex("f95e59"))
      | Blue => backgroundColor(`hex("00bcea"))
      | Yellow => backgroundColor(`hex("f4ed7c"))
      };

    style([bgColor, ...baseStyle]);
  };
};

let component = ReasonReact.reducerComponent("App");

let make = _children => {
  ...component,
  initialState: () => {sequence: [||]},
  reducer: (action, _state) =>
    switch (action) {
    | SetSequence(array) => ReasonReact.Update({sequence: array})
    },
  didMount: self => {
    let array =
      Belt.Array.makeBy(20, _i =>
        Js.Math.floor(Js.Math.random() *. 4.0 +. 1.0)
      );
    self.send(SetSequence(array));
    ();
  },
  render: self =>
    <div className=Styles.container>
      <div className=Styles.boxes>
        <div
          className={Styles.box(Green)}
          onClick={_e => Sounds.green##play()}
        />
        <div className={Styles.box(Red)} onClick={_e => Sounds.red##play()} />
        <div
          className={Styles.box(Blue)}
          onClick={_e => Sounds.blue##play()}
        />
        <div
          className={Styles.box(Yellow)}
          onClick={_e => Sounds.yellow##play()}
        />
      </div>
    </div>,
};

let default = ReasonReact.wrapReasonForJs(~component, _jsProps => make([||]));
