
export const name = 'LpSource';

export const caption = 'Lp Source';

export const inputs = [
];

export const parameters = [
    { id: "param1", type: 'spectrum', typeName: "Lp", description: 'Output Lp'},
];

export const readonly_outputs = [
];

export const outputs = [
    { id: "out1", type: 'spectrum', typeName: "Lp", description: 'Output Lp' },
];

export function calculate (inputs, parameters) {
    return {
        "out1": parameters["param1"]
    };
};

