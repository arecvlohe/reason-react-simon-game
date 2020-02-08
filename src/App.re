type sequence = array(int);

type state = {sequence};

type action =
  | SetSequence(sequence);

[@react.component]
let make = () => {
  let (state, send) =
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

  <div>
    {state.sequence
     |> Array.map(number =>
          <span key={Js.Math.random() |> Js.Float.toString}>
            {number |> string_of_int |> ReasonReact.string}
          </span>
        )
     |> React.array}
  </div>;
};
