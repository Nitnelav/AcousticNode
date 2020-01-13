export const name = 'RectangularElbowWithDivision';

export const caption = 'Rectangular Elbow With Division';

export const description = 'A simple inplementation of a Rectangular Elbow  With Division attenuation and regeneration';

export const inputs = [
    { id: "Lw", type: 'spectrum', typeName: "Lw", description: 'Input Lw' },
];

export const parameters = [
    { id: "flow1", type: 'int', typeName: "Flow", description: 'Primary Air Flow (m3/h)', default: 1000 },
    { id: "width1", type: 'int', typeName: "Width", description: 'Primary Duct Width (mm)', default: 200 },
    { id: "height1", type: 'int', typeName: "Height", description: 'Primary Duct Height (mm)', default: 200 },
    { id: "flow2", type: 'int', typeName: "Flow", description: 'Secondary Air Flow (m3/h)', default: 1000 },
    { id: "width2", type: 'int', typeName: "Width", description: 'Secondary Duct Width (mm)', default: 200 },
    { id: "height2", type: 'int', typeName: "Height", description: 'Secondary Duct Height (mm)', default: 200 },
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

const regDiff = [-5, -6, -7, -10, -12, -15, -19, -19]; // extrapolation : 8kHz = 4kHz

export function calculate (inputs, parameters) {
    const width1 = parameters["width1"];
    const height1 = parameters["height1"];
    const flow1 = parameters["flow1"];
    const width2 = parameters["width2"];
    const height2 = parameters["height2"];
    const flow2 = parameters["flow2"];

    var pp = [];
    var attenuation = [];
    var regeneration = [];

    var ppCorr = Math.min(10 * Math.log10(1 + (width2 * height2) / (width1 * height1)), 10);
    var regCorr = 40 * Math.log10(flow2 / flow1) + 40 * Math.log10((width1 * height1) / (width2 * height2));

    if (parameters["coating"] === "yes") {
        if (height1 > 500) {
            pp = attCoat;
        } else if (height1 > 250) {
            pp = [0].concat(attCoat);
        } else {
            pp = [0, 0].concat(attCoat);
        }
    } else {
        if (height1 > 1000) {
            pp = att;
        } else if (height1 > 500) {
            pp = [0].concat(att);
        } else {
            pp = [0, 0].concat(att);
        }
    }
    pp.length = 8;

    var Lw = 17 + 60 * Math.log10(flow1 / (3600 * width1 * height1 * 0.000001)) + 15 * Math.log10(width1 * height1 * 0.000001);

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

