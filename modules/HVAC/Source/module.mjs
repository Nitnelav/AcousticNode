
export const name = 'LwSource';

export const caption = 'Lw Source';

export const inputs = [
];

export const parameters = [
    { id: "param1", type: 'spectrum', typeName: "Lw", description: 'Output Lw'},
];

export const outputs = [
    { id: "out1", type: 'spectrum', typeName: "Lw", description: 'Output Lw' },
];

export function calculate (inputs, parameters) {
    var outputs = {};
    outputs["out1"] = parameters["param1"];
    return outputs;
};

