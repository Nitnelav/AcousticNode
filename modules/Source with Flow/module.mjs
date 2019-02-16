
export const name = 'LwSourceFlow';

export const caption = 'Lw Source w/ Flow';

export const inputs = [
];

export const parameters = [
    { type: 'Lw', description: 'Output Lw'},
    { type: 'float', description: 'Flow rate (m3/s)', default: 100.0 },
];

export const outputs = [
    { type: 'Lw', description: 'Output Lw' },
    { type: 'Flow', description: 'Air flow rate' },
];

export function calculate (inputs, parameters) {
    var outputLw = parameters[0];
    var flowRate = parameters[1];
    return [ outputLw, flowRate ];
};

