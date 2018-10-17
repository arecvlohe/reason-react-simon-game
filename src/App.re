type sequence = list(Types.colors);

type state = {
  sequence,
  level: int,
  active: option(Types.colors),
  input: list(Types.colors),
  isPlaying: bool,
  isStrict: bool,
};

type action =
  | SetSequence(sequence)
  | PlaySequence
  | PlaySound(Types.colors)
  | ResetColor
  | Input(Types.colors)
  | CheckInput
  | Playback(bool)
  | Reset
  | SetStrict;

module Styles = {
  open Css;
  global(
    "body",
    [
      fontFamily(
        "-apple-system,BlinkMacSystemFont,\"Segoe UI\",Roboto,Oxygen-Sans,Ubuntu,Cantarell,\"Helvetica Neue\",sans-serif",
      ),
    ],
  );
  let container =
    style([
      display(`flex),
      justifyContent(`center),
      alignItems(`center),
      minHeight(`vh(100.0)),
      flexDirection(`column),
    ]);
  let boxes =
    style([
      display(`flex),
      flexWrap(`wrap),
      maxWidth(`px(500)),
      maxHeight(`px(500)),
    ]);
  let box = (~bgColor: Types.colors, ~active: option(Types.colors)) => {
    let baseStyle = [minHeight(`px(250)), minWidth(`px(250))];

    let opacity =
      switch (bgColor, active) {
      | (Green, Some(Green)) => opacity(0.5)
      | (Red, Some(Red)) => opacity(0.5)
      | (Blue, Some(Blue)) => opacity(0.5)
      | (Yellow, Some(Yellow)) => opacity(0.5)
      | (_, None) => opacity(1.0)
      | (_, Some(_)) => opacity(1.0)
      };

    let bgColor =
      switch (bgColor) {
      | Green => backgroundColor(`hex("07f767"))
      | Red => backgroundColor(`hex("f95e59"))
      | Blue => backgroundColor(`hex("00bcea"))
      | Yellow => backgroundColor(`hex("f4ed7c"))
      };

    style([bgColor, opacity, ...baseStyle]);
  };
  let controls = style([marginTop(`px(10)), textAlign(`center)]);
  let button = style([fontSize(`px(14))]);
  let buttons = style([marginTop(`px(10))]);
};

let component = ReasonReact.reducerComponent("App");

let make = _children => {
  ...component,
  initialState: () => {
    sequence: [],
    level: 1,
    active: None,
    input: [],
    isPlaying: false,
    isStrict: false,
  },
  reducer: (action, state) =>
    switch (action) {
    | SetSequence(list) => ReasonReact.Update({...state, sequence: list})
    | PlaySequence =>
      let l =
        Belt.List.take(state.sequence, state.level)
        ->Belt.Option.getWithDefault([]);
      ReasonReact.UpdateWithSideEffects(
        {...state, isPlaying: true},
        (
          self => {
            Belt.List.forEachWithIndex(
              l,
              (index, color) => {
                let _id =
                  Js.Global.setTimeout(
                    () => self.send(PlaySound(color)),
                    (index + 1) * 1000,
                  );
                ();
              },
            );
            let _id =
              Js.Global.setTimeout(
                () => self.send(Playback(false)),
                state.level * 1000 + 300,
              );
            ();
          }
        ),
      );
    | PlaySound(color) =>
      ReasonReact.UpdateWithSideEffects(
        {...state, active: Some(color)},
        (
          self => {
            let sound =
              Belt.List.getAssoc(Sounds.map, color, (==))
              ->Belt.Option.getWithDefault(Sounds.green);
            sound##play();
            let _id = Js.Global.setTimeout(() => self.send(ResetColor), 300);
            ();
          }
        ),
      )
    | ResetColor => ReasonReact.Update({...state, active: None})
    | Input(color) =>
      ReasonReact.UpdateWithSideEffects(
        {...state, input: [color, ...state.input]},
        (self => self.send(CheckInput)),
      )
    | CheckInput =>
      let {level, input, sequence, isStrict} = state;
      let currentUserColor = Belt.List.headExn(input);
      let inputLength = Belt.List.length(input);
      let currentSequenceColor = Belt.List.getExn(sequence, inputLength - 1);

      switch (
        currentUserColor === currentSequenceColor,
        inputLength === level,
        isStrict,
      ) {
      | (false, _, false) =>
        ReasonReact.UpdateWithSideEffects(
          {...state, input: []},
          (
            self => {
              Sounds.error##play();
              self.send(PlaySequence);
            }
          ),
        )
      | (false, _, true) =>
        ReasonReact.UpdateWithSideEffects(
          {...state, input: [], level: 1},
          (
            self => {
              Sounds.error##play();
              self.send(PlaySequence);
            }
          ),
        )
      | (true, false, _) =>
        ReasonReact.SideEffects(
          (self => self.send(PlaySound(currentUserColor))),
        )
      | (true, true, _) =>
        ReasonReact.UpdateWithSideEffects(
          {...state, input: [], level: state.level + 1},
          (
            self => {
              self.send(PlaySound(currentUserColor));
              self.send(PlaySequence);
            }
          ),
        )
      };
    | Playback(bool) => ReasonReact.Update({...state, isPlaying: bool})
    | Reset =>
      ReasonReact.UpdateWithSideEffects(
        {...state, level: 1},
        (self => self.send(PlaySequence)),
      )
    | SetStrict => ReasonReact.Update({...state, isStrict: !state.isStrict})
    },
  didMount: self => {
    let list =
      Belt.List.makeBy(
        20,
        _i => {
          open Types;
          let num = Js.Math.floor(Js.Math.random() *. 4.0 +. 1.0);
          switch (num) {
          | 1 => Green
          | 2 => Red
          | 3 => Blue
          | 4 => Yellow
          | _ => Green
          };
        },
      );
    self.send(SetSequence(list));
    ();
  },
  render: self => {
    let {level, active, isPlaying, isStrict} = self.state;
    <div className=Styles.container>
      <h1> "Simon Game in ReasonReact"->ReasonReact.string </h1>
      <div className=Styles.boxes>
        <div
          className={Styles.box(~bgColor=Green, ~active)}
          onClick={_e => self.send(Input(Green))}
        />
        <div
          className={Styles.box(~bgColor=Red, ~active)}
          onClick={_e => self.send(Input(Red))}
        />
        <div
          className={Styles.box(~bgColor=Blue, ~active)}
          onClick={_e => self.send(Input(Blue))}
        />
        <div
          className={Styles.box(~bgColor=Yellow, ~active)}
          onClick={_e => self.send(Input(Yellow))}
        />
      </div>
      <div className=Styles.controls>
        <div>
          <span> "Strict"->ReasonReact.string </span>
          <input
            type_="checkbox"
            checked=isStrict
            onInput={_e => self.send(SetStrict)}
          />
        </div>
        <div> {{j|Level: $level|j} |> ReasonReact.string} </div>
        <div className=Styles.buttons>
          <button
            className=Styles.button
            onClick={_e => self.send(PlaySequence)}
            disabled=isPlaying>
            {"Start" |> ReasonReact.string}
          </button>
          <button
            className=Styles.button
            onClick={_e => self.send(Reset)}
            disabled=isPlaying>
            "Reset"->ReasonReact.string
          </button>
        </div>
      </div>
    </div>;
  },
};

let default = ReasonReact.wrapReasonForJs(~component, _jsProps => make([||]));
