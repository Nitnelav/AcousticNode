export const name = 'CircularElbow';

export const caption = 'Circular Elbow';

export const description = 'A simple inplementation of a Circular Elbow attenuation and regeneration';

export const inputs = [
    { id: "Lw", type: 'spectrum', typeName: "Lw", description: 'Input Lw' },
];

export const parameters = [
    { id: "flow", type: 'int', typeName: "Flow", description: 'Air Flow (m3/h)', default: 1000 },
    { id: "diameter", type: 'int', typeName: "Diameter", description: 'Duct Diameter (mm)', default: 200 },
    { id: "coating", type: 'choice', typeName: "Coating", description: 'With Coating ?', choices: ["yes", "no"], default: "no" },
];

export const readonly_outputs = [
    { id: "attenuation", type: 'spectrum', typeName: "Lw", description: 'Attenuation' },
    { id: "regeneration", type: 'spectrum', typeName: "Lw", description: 'Regeneration' },
];

export const outputs = [
    { id: "Lw", type: 'spectrum', typeName: "Lw", description: 'Output Lw' },
];

const att = [0, 1, 2, 3, 3, 3, 3, 3]; // extrapolation : 8kHz = 4kHz
const attCoat = [0, 3, 4, 4, 5, 6, 6, 6]; // extrapolation : 8kHz = 4kHz

const regDiff = [-3, -5, -7, -10, -12, -15, -19, -19]; // extrapolation : 8kHz = 4kHz

export function calculate (inputs, parameters) {
    const diameter = parameters["diameter"];
    const flow = parameters["flow"];

    var attenuation = [];
    var regeneration = [];

    if (parameters["coating"] === "yes") {
        if (diameter > 500) {
            attenuation = attCoat;
        } else if (diameter > 250) {
            attenuation = [0].concat(attCoat);
        } else {
            attenuation = [0, 0].concat(attCoat);
        }
    } else {
        if (diameter > 1000) {
            attenuation = att;
        } else if (diameter > 500) {
            attenuation = [0].concat(att);
        } else {
            attenuation = [0, 0].concat(att);
        }
    }    
    attenuation.length = 8;

    var Lw = 20 + 50 * Math.log10(flow / (3600 * Math.PI * Math.pow(diameter * 0.001, 2) / 4)) + 10 * Math.log10(Math.pow(diameter, 2) * 0.000001 * Math.PI / 4);
    
    for (let freq = 0; freq < 8; freq++) {
        regeneration[freq] = Math.round(Math.max(Lw + regDiff[freq], 0));
    }

    var output = [];
    for (let freq = 0; freq < 8; freq++) {
        output[freq] = inputs["Lw"][freq] - attenuation[freq] + regeneration[freq];
    }
    return {
        "Lw": output,
        "attenuation": attenuation,
        "regeneration": regeneration,
    };
};

