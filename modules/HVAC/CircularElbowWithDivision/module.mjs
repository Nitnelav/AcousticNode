export const name = 'CircularElbowWithDivision';

export const caption = 'Circular Elbow With Division';

export const description = 'A simple inplementation of a Circular Elbow  With Division attenuation and regeneration';

export const inputs = [
    { id: "Lw", type: 'spectrum', typeName: "Lw", description: 'Input Lw' },
];

export const parameters = [
    { id: "flow1", type: 'int', typeName: "Flow", description: 'Primary Air Flow (m3/h)', default: 1000 },
    { id: "diameter1", type: 'int', typeName: "Diameter", description: 'Primary Duct Diameter (mm)', default: 200 },
    { id: "flow2", type: 'int', typeName: "Flow", description: 'Secondary Air Flow (m3/h)', default: 1000 },
    { id: "diameter2", type: 'int', typeName: "Diameter", description: 'Secondary Duct Diameter (mm)', default: 200 },
    { id: "coating", type: 'choice', typeName: "Coating", description: 'With Coating ?', choices: ["yes", "no"], default: "no" },
];

export const readonly_outputs = [
    { id: "attenuation", type: 'spectrum', typeName: "Lw", description: 'Attenuation' },
    { id: "regeneration", type: 'spectrum', typeName: "Lw", description: 'Regeneration' },
];

export const outputs = [
    { id: "Lw1", type: 'spectrum', typeName: "Lw", description: 'Primary Output Lw' },
    { id: "Lw2", type: 'spectrum', typeName: "Lw", description: 'Secondary Output Lw' },
];

const att = [0, 1, 2, 3, 3, 3, 3, 3]; // extrapolation : 8kHz = 4kHz
const attCoat = [0, 3, 4, 4, 5, 6, 6, 6]; // extrapolation : 8kHz = 4kHz

const regDiff = [-3, -5, -7, -10, -12, -15, -19, -19]; // extrapolation : 8kHz = 4kHz

export function calculate (inputs, parameters) {
    const diameter1 = parameters["diameter1"];
    const flow1 = parameters["flow1"];
    const diameter2 = parameters["diameter2"];
    const flow2 = parameters["flow2"];

    var pp = [];
    var attenuation = [];
    var regeneration = [];

    var ppCorr = Math.min(10 * Math.log10(1 + (diameter2 * diameter2) / (diameter1 * diameter1)), 10);
    var regCorr = 40 * Math.log10(flow2 / flow1) + 40 * Math.log10((diameter1 * diameter1) / (diameter2 * diameter2));

    if (parameters["coating"] === "yes") {
        if (diameter1 > 500) {
            pp = attCoat;
        } else if (diameter1 > 250) {
            pp = [0].concat(attCoat);
        } else {
            pp = [0, 0].concat(attCoat);
        }
    } else {
        if (diameter1 > 1000) {
            pp = att;
        } else if (diameter1 > 500) {
            pp = [0].concat(att);
        } else {
            pp = [0, 0].concat(att);
        }
    }    
    pp.length = 8;

    var Lw = 20 + 50 * Math.log10(flow1 / (3600 * Math.PI * Math.pow(diameter1 * 0.001, 2) / 4)) + 10 * Math.log10(Math.pow(diameter1, 2) * 0.000001 * Math.PI / 4);

    for (let freq = 0; freq < 8; freq++) {
        attenuation[freq] = Math.round(pp[freq] + ppCorr);
        regeneration[freq] = Math.round(Math.max(Lw + regDiff[freq] + regCorr, 0));
    }

    var output = [];
    for (let freq = 0; freq < 8; freq++) {
        output[freq] = inputs["Lw"][freq] - attenuation[freq] + regeneration[freq];
    }
    return {
        "Lw1": output,
        "Lw2": output,
        "attenuation": attenuation,
        "regeneration": regeneration,
    };
};

