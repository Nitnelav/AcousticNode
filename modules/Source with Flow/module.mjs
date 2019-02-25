
export const name = 'LwSourceFlow';

export const caption = 'Lw Source w/ Flow';

export const inputs = [
];

export const parameters = [
    { type: 'spectrum', typeName: "Lw", description: 'Output Lw' },
    { type: 'int', typeName: "Flow", description: 'Flow rate (m3/s)', default: 100.0 },
];

export const outputs = [
    { type: 'spectrum', typeName: "Lw", description: 'Output Lw' },
    { type: 'int', typeName: "Flow", description: 'Air flow rate' },
];

export function calculate (inputs, parameters) {
    var outputLw = parameters[0];
    var flowRate = parameters[1];
    return [ outputLw, flowRate ];
};

