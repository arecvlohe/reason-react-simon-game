open Webapi.Dom;

type sequence = list(Types.colors);

type state = {
  sequence,
  level: int,
  active: option(Types.colors),
  input: list(Types.colors),
  isStrict: bool,
  isPlaying: bool,
};

type action =
  | SetSequence(sequence)
  | PlaySequence
  | PlaySound(Types.colors)
  | ResetColor
  | Input(Types.colors)
  | CheckInput
  | Reset
  | SetStrictness
  | SetPlaying;

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
    let baseStyle = [
      minHeight(`px(250)),
      minWidth(`px(250)),
      border(`px(0), `none, `transparent),
    ];

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
  let buttons = style([marginTop(`px(10))]);
};

let makeSequence = (~len=5, ()) =>
  Belt.List.makeBy(
    len,
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

let component = ReasonReact.reducerComponent("App");

let make = _children => {
  ...component,
  initialState: () => {
    sequence: [],
    level: 1,
    active: None,
    input: [],
    isStrict: false,
    isPlaying: false,
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
                () => self.send(SetPlaying),
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
      let isEnd = inputLength === Belt.List.length(sequence);
      switch (
        currentUserColor === currentSequenceColor,
        inputLength === level,
        isStrict,
        isEnd,
      ) {
      | (false, _, false, _) =>
        ReasonReact.UpdateWithSideEffects(
          {...state, input: []},
          (
            self => {
              Sounds.error##play();
              self.send(PlaySequence);
            }
          ),
        )
      | (false, _, true, _) =>
        ReasonReact.UpdateWithSideEffects(
          {...state, input: [], level: 1},
          (
            self => {
              Sounds.error##play();
              self.send(PlaySequence);
            }
          ),
        )
      | (true, false, _, false) =>
        ReasonReact.SideEffects(
          (self => self.send(PlaySound(currentUserColor))),
        )
      | (true, true, _, false) =>
        ReasonReact.UpdateWithSideEffects(
          {...state, input: [], level: state.level + 1},
          (
            self => {
              self.send(PlaySound(currentUserColor));
              self.send(PlaySequence);
            }
          ),
        )
      | (true, _, _, true) =>
        let list = makeSequence();
        ReasonReact.UpdateWithSideEffects(
          {...state, input: [], level: 1, sequence: list},
          (
            self => {
              self.send(PlaySound(currentUserColor));
              let _id =
                Js.Global.setTimeout(
                  () => Window.alert("You won!", window),
                  400,
                );
              ();
            }
          ),
        );
      };
    | Reset => ReasonReact.Update({...state, input: [], level: 1})
    | SetStrictness =>
      ReasonReact.Update({...state, isStrict: !state.isStrict})
    | SetPlaying =>
      ReasonReact.Update({...state, isPlaying: !state.isPlaying})
    },
  didMount: self => {
    let list = makeSequence();
    self.send(SetSequence(list));
    ();
  },
  render: self => {
    let {level, active, isStrict, isPlaying} = self.state;
    <div className=Styles.container>
      <h1> "Simon Game in ReasonReact"->ReasonReact.string </h1>
      <div className=Styles.boxes>
        <button
          type_="button"
          className={Styles.box(~bgColor=Green, ~active)}
          onClick={_e => self.send(Input(Green))}
          disabled=isPlaying
        />
        <button
          type_="button"
          className={Styles.box(~bgColor=Red, ~active)}
          onClick={_e => self.send(Input(Red))}
          disabled=isPlaying
        />
        <button
          type_="button"
          className={Styles.box(~bgColor=Blue, ~active)}
          onClick={_e => self.send(Input(Blue))}
          disabled=isPlaying
        />
        <button
          type_="button"
          className={Styles.box(~bgColor=Yellow, ~active)}
          onClick={_e => self.send(Input(Yellow))}
          disabled=isPlaying
        />
      </div>
      <div className=Styles.controls>
        <div>
          <span> "Strict"->ReasonReact.string </span>
          <input
            type_="checkbox"
            checked=isStrict
            onChange={_e => self.send(SetStrictness)}
          />
        </div>
        <div> {{j|Level: $level|j} |> ReasonReact.string} </div>
        <div className=Styles.buttons>
          <button onClick={_e => self.send(PlaySequence)} disabled=isPlaying>
            {"Start" |> ReasonReact.string}
          </button>
          <button onClick={_e => self.send(Reset)} disabled=isPlaying>
            "Reset"->ReasonReact.string
          </button>
        </div>
      </div>
    </div>;
  },
};

let default = ReasonReact.wrapReasonForJs(~component, _jsProps => make([||]));
