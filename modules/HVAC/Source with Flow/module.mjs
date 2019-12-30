
export const name = 'LwSourceFlow';

export const caption = 'Lw Source w/ Flow';

export const inputs = [
];

export const parameters = [
    { id: "param1", type: 'spectrum', typeName: "Lw", description: 'Output Lw' },
    { id: "param2", type: 'int', typeName: "Flow", description: 'Flow rate (m3/s)', default: 100.0 },
];

export const outputs = [
    { id: "out1", type: 'spectrum', typeName: "Lw", description: 'Output Lw' },
    { id: "out2", type: 'int', typeName: "Flow", description: 'Air flow rate' },
];

export function calculate (inputs, parameters) {
    var outputs = {};
    outputs["out1"] = parameters["param1"];
    outputs["out2"] = parameters["param2"];
    return outputs;
};

