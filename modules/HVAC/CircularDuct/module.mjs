export const name = 'CircularDuct';

export const caption = 'Circular Duct';

export const description = 'A simple inplementation of a Circular Duct attenuation';

export const inputs = [
    { id: "in1", type: 'spectrum', typeName: "Lw", description: 'Input Lw'},
];

export const parameters = [
    { id: "length", type: 'int', typeName: "Int", description: 'Duct Length (mm)', default: 2000 },
    { id: "diameter", type: 'int', typeName: "Int", description: 'Duct Diameter (mm)', default: 200 },
    { id: "coating", type: 'choice', typeName: "Coating", description: 'Coating', choices: ["None", "12.5mm", "25mm"], default: "None" }
];

export const readonly_outputs = [
    { id: "attenuation", type: 'spectrum', typeName: "Lw", description: 'Attenuation' },
];

export const outputs = [
    { id: "Lw", type: 'spectrum', typeName: "Lw", description: 'Output Lw' },
];

const alpha200 = [1, 1, 1, 1, 3, 3, 3, 3]; // extrapolation : 8kHz = 4kHz
const alpha400 = [0.5, 1, 1, 1, 2, 2, 2, 2]; // extrapolation : 8kHz = 4kHz
const alpha800 = [0.3, 0.6, 0.6, 1, 1, 1, 1, 1]; // extrapolation : 8kHz = 4kHz
const alpha1600 = [0.3, 0.3, 0.3, 0.5, 0.5, 0.5, 0.5, 0.5]; // extrapolation : 8kHz = 4kHz

const coat12p5 = [12, 15, 18, 24, 44, 60, 80, 80]; // extrapolation : 8kHz = 4kHz
const coat25 = [24, 27, 29, 46, 68, 77, 82, 82]; // extrapolation : 8kHz = 4kHz

export function calculate (inputs, parameters) {
    const length = parameters["length"];
    const diameter = parameters["diameter"];
    const coating = parameters["coating"];
    
    var attenuation = [];

    if (coating === "None") {
        var alpha = [];
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
            attenuation[freq] = Math.round(Math.min(0.1 * alpha[freq] * length * 0.001, 25));
        }
    } else {
        var coat = [];
        if (coating === "12.5mm") {
            coat = coat12p5;
        } else {
            coat = coat25;
        }

        for (let freq = 0; freq < 8; freq++) {
            attenuation[freq] = Math.round(Math.min(40 * coat[freq] * length * 0.001 / diameter, 25));
        }
    }

    var output = [];
    for (let freq = 0; freq < 8; freq++) {
        output[freq] = inputs["in1"][freq] - attenuation[freq];
    }
    return {
        "Lw": output,
        "attenuation": attenuation
    };
};

