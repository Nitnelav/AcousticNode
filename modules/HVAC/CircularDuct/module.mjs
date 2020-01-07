export const name = 'CircularDuct';

export const caption = 'Circular Duct';

export const description = 'A simple inplementation of a Circular Duct attenuation';

export const inputs = [
    { id: "in1", type: 'spectrum', typeName: "Lw", description: 'Input Lw'},
];

export const parameters = [
    { id: "length", type: 'int', typeName: "Int", description: 'Duct Length (mm)', default: 2000 },
    { id: "diameter", type: 'int', typeName: "Int", description: 'Duct Diameter (mm)', default: 200 }
];

export const outputs = [
    { id: "Lw", type: 'spectrum', typeName: "Lw", description: 'Output Lw' },
];

const alpha200 = [1, 1, 1, 1, 3, 3, 3, 3]; // extrapolation : 8kHz = 4kHz
const alpha400 = [0.5, 1, 1, 1, 2, 2, 2, 2]; // extrapolation : 8kHz = 4kHz
const alpha800 = [0.3, 0.6, 0.6, 1, 1, 1, 1]; // extrapolation : 8kHz = 4kHz
const alpha1600 = [0.3, 0.3, 0.3, 0.5, 0.5, 0.5, 0.5]; // extrapolation : 8kHz = 4kHz

export function calculate (inputs, parameters) {
    var outputs = {};
    const length = parameters["length"];
    const diameter = parameters["diameter"];
    var alpha = [];
    var attenuation = [];

    if (diameter > 800) {
        alpha = alpha1600;
    } else if (diameter > 400) {
        alpha = alpha800;
    } else if (diameter > 200) {
        alpha = alpha400;
    } else {
        alpha = alpha200;
    }
    
    for (let freq = 0; freq < 8; freq++) {
        attenuation[freq] = Math.min(0.1 * alpha[freq] * length * 0.001, 25);
    }

    var outputLw = [];
    for (let freq = 0; freq < 8; freq++) {
        outputLw[freq] = inputs["in1"][freq] - attenuation[freq];
    }
    outputs["Lw"] = outputLw;
    return outputs;
};

