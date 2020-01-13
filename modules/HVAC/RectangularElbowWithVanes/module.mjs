export const name = 'RectangularElbowWithVanes';

export const caption = 'Rectangular Elbow With Vanes';

export const description = 'A simple inplementation of a Rectangular Elbow With Vanes attenuation and regeneration';

export const inputs = [
    { id: "Lw", type: 'spectrum', typeName: "Lw", description: 'Input Lw' },
];

export const parameters = [
    { id: "flow", type: 'int', typeName: "Flow", description: 'Air Flow (m3/h)', default: 1000 },
    { id: "width", type: 'int', typeName: "Width", description: 'Duct Width (mm)', default: 200 },
    { id: "height", type: 'int', typeName: "Height", description: 'Duct Height (mm)', default: 200 },
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

const regDiff = [-5, -6, -7, -10, -12, -15, -19, -19]; // extrapolation : 8kHz = 4kHz

export function calculate (inputs, parameters) {
    const width = parameters["width"];
    const height = parameters["height"];
    const flow = parameters["flow"];

    var attenuation = [];
    var regeneration = [];

    if (parameters["coating"] === "yes") {
        if (height > 500) {
            attenuation = attCoat;
        } else if (height > 250) {
            attenuation = [0].concat(attCoat);
        } else {
            attenuation = [0, 0].concat(attCoat);
        }
    } else {
        if (height > 1000) {
            attenuation = att;
        } else if (height > 500) {
            attenuation = [0].concat(att);
        } else {
            attenuation = [0, 0].concat(att);
        }
    }    
    attenuation.length = 8;

    var Lw = 17 + 60 * Math.log10(flow / (3600 * width * height * 0.000001)) + 15 * Math.log10(width * height * 0.000001);
    
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

