export const name = 'LwCorrection';

export const caption = 'Lw Correction';

export const inputs = [
    { type: 'spectrum', typeName: "Lw", description: 'Input Lw'},
];

export const parameters = [
    { type: 'spectrum', typeName: "Lw", description: 'Correction Lw'},
];

export const outputs = [
    { type: 'spectrum', typeName: "Lw", description: 'Output Lw' },
];

export function calculate (inputs, parameters) {
    var outputLw = [];
    for (let freq = 0; freq < 8; freq++) {
        outputLw[freq] = inputs[0][freq] + parameters[0][freq];
    }
    return [ outputLw ];
};

