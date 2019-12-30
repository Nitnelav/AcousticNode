export const name = 'LwCorrection';

export const caption = 'Lw Correction';

export const inputs = [
    { id: "in1", type: 'spectrum', typeName: "Lw", description: 'Input Lw'},
];

export const parameters = [
    { id: "param1", type: 'spectrum', typeName: "Lw", description: 'Correction Lw'},
];

export const outputs = [
    { id: "out1", type: 'spectrum', typeName: "Lw", description: 'Output Lw' },
];

export function calculate (inputs, parameters) {
    var outputs = {};
    var outputLw = [];
    for (let freq = 0; freq < 8; freq++) {
        outputLw[freq] = inputs["in1"][freq] + parameters["param1"][freq];
    }
    outputs["out1"] = outputLw;
    return outputs;
};

