type sequence = array(int);

type state = {sequence};

type action =
  | SetSequence(sequence);

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
    <div>
      {
        self.state.sequence
        |> Array.map(number =>
             <span key={Js.Math.random() |> string_of_float}>
               {number |> string_of_int |> ReasonReact.string}
             </span>
           )
        |> ReasonReact.array
      }
    </div>,
};

let default = ReasonReact.wrapReasonForJs(~component, _jsProps => make([||]));
