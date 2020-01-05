export const name = 'LwCorrection';

export const caption = 'Lw Correction';

export const inputs = [
    { id: "in1", type: 'spectrum', typeName: "Lw", description: 'Input Lw'},
];

export const parameters = [
    { id: "correction", type: 'spectrum', typeName: "Lw", description: 'Correction Lw'},
    { id: "choice", type: 'choice', typeName: "Choice", description: 'Add or Substract', choices: ["add", "substract"], default: "add"},
];

export const outputs = [
    { id: "out1", type: 'spectrum', typeName: "Lw", description: 'Output Lw' },
];

export function calculate (inputs, parameters) {
    var outputs = {};
    var outputLw = [];
    var addOrSub = parameters["choice"] === "add" ? 1 : -1;
    for (let freq = 0; freq < 8; freq++) {
        outputLw[freq] = inputs["in1"][freq] + addOrSub * parameters["correction"][freq];
    }
    outputs["out1"] = outputLw;
    return outputs;
};

