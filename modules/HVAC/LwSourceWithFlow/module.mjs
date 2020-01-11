
export const name = 'LwSourceWithFlow';

export const caption = 'Lw Source With Flow';

export const inputs = [
];

export const parameters = [
    { id: "param1", type: 'spectrum', typeName: "Lw", description: 'Lw'},
    { id: "param2", type: 'int', typeName: "Flow", description: 'Flow (m3/h)'},
];

export const readonly_outputs = [
];

export const outputs = [
    { id: "out1", type: 'spectrum', typeName: "Lw", description: 'Output Lw' },
    { id: "out2", type: 'int', typeName: "Flow", description: 'Output Flow' },
];

export function calculate (inputs, parameters) {
    return {
        "out1": parameters["param1"],
        "out2": parameters["param2"]
    };
};

