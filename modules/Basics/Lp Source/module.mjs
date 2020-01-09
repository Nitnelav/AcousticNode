
export const name = 'LpSource';

export const caption = 'Lp Source';

export const inputs = [
];

export const parameters = [
    { id: "param1", type: 'spectrum', typeName: "Lp", description: 'Output Lp'},
];

export const outputs = [
    { id: "out1", type: 'spectrum', typeName: "Lp", description: 'Output Lp' },
];

export function calculate (inputs, parameters) {
    var outputs = {};
    outputs["out1"] = parameters["param1"];
    return outputs;
};

