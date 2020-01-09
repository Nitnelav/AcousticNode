
export const name = 'AlphaSource';

export const caption = 'Alpha Source';

export const inputs = [
];

export const parameters = [
    { id: "param1", type: 'spectrum', typeName: "Alpha", description: 'Output Alpha'},
];

export const outputs = [
    { id: "out1", type: 'spectrum', typeName: "Alpha", description: 'Output Alpha' },
];

export function calculate (inputs, parameters) {
    var outputs = {};
    outputs["out1"] = parameters["param1"];
    return outputs;
};

