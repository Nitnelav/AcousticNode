
export const name = 'AlphaSource';

export const caption = 'Alpha Source';

export const inputs = [
];

export const parameters = [
    { id: "param1", type: 'spectrum', typeName: "Alpha", description: 'Output Alpha'},
];

export const readonly_outputs = [
];

export const outputs = [
    { id: "out1", type: 'spectrum', typeName: "Alpha", description: 'Output Alpha' },
];

export function calculate (inputs, parameters) {
    return {
        "out1": parameters["param1"]
    };
};

