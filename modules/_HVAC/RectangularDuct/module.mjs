export const name = 'RectangularDuct';

export const caption = 'Rectangular Duct';

export const description = 'A simple inplementation of a Rectangular Duct attenuation';

export const inputs = [
    { id: "in1", type: 'spectrum', typeName: "Lw", description: 'Input Lw'},
];

export const parameters = [
    { id: "length", type: 'int', typeName: "Int", description: 'Duct Length (mm)', default: 2000 },
    { id: "width", type: 'int', typeName: "Int", description: 'Duct Width (mm)', default: 200 },
    { id: "height", type: 'int', typeName: "Int", description: 'Duct Height (mm)', default: 200 }
];

export const outputs = [
    { id: "Lw", type: 'spectrum', typeName: "Lw", description: 'Output Lw' },
];

const alpha200 = [6, 6, 4, 3, 3, 3, 3, 3]; // extrapolation : 8kHz = 4kHz
const alpha400 = [6, 5, 4, 3, 2, 2, 2, 2]; // extrapolation : 8kHz = 4kHz
const alpha800 = [5, 4, 3, 1, 1, 1, 1, 1]; // extrapolation : 8kHz = 4kHz
const alpha1600 = [4, 3, 1, 1, 0.5, 0.5, 0.5, 0.5]; // extrapolation : 8kHz = 4kHz

export function calculate (inputs, parameters) {
    var outputs = {};
    const length = parameters["length"];
    const width = parameters["width"];
    const height = parameters["height"];
    var attenuation = [];

    var diameter = Math.sqrt(width * height);

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

