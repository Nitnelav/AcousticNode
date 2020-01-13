export const name = 'FlexibleDuctWithCoating';

export const caption = 'Flexible Duct With Coating';

export const description = 'A simple inplementation of a Flexible Duct With Coating attenuation';

export const inputs = [
    { id: "in1", type: 'spectrum', typeName: "Lw", description: 'Input Lw'},
];

export const parameters = [
    { id: "length", type: 'choice', typeName: "Int", description: 'Duct Length (mm)', choices: ["1000", "1500", "2000"], default: "1000" },
    { id: "diameter", type: 'choice', typeName: "Int", description: 'Duct Diameter (mm)', choices: ["80", "100", "125", "160", "200", "250", "315", "450", "500"], default: "80" },
    { id: "thickness", type: 'choice', typeName: "Thickness", description: 'Coating Thickness (mm)', choices: ["12.5", "25"], default: "12.5" }
];

export const readonly_outputs = [
    { id: "attenuation", type: 'spectrum', typeName: "Lw", description: 'Attenuation' },
];

export const outputs = [
    { id: "Lw", type: 'spectrum', typeName: "Lw", description: 'Output Lw' },
];

// extrapolation : 8kHz = 4kHz
const attenuation = {
    "12.5": {
        "1000": {
            "80": [4, 11, 10, 16, 24, 38, 27, 27],
            "100": [0, 5, 8, 19, 35, 30, 19, 19],
            "125": [0, 2, 5, 9, 17, 24, 19, 19],
            "160": [0, 4, 7, 11, 20, 36, 16, 16],
            "200": [0, 2, 6, 12, 21, 10, 8, 8],
            "250": [0, 3, 6, 11, 12, 8, 11, 11],
            "315": [0, 3, 6, 11, 15, 9, 9, 9],
            "450": [0, 1, 3, 6, 9, 6, 8, 8],
            "500": [0, 1, 4, 9, 8, 6, 7, 7],
        },
        "1500": {
            "80": [4, 11, 12, 21, 36, 44, 34, 34],
            "100": [0, 4, 10, 22, 41, 40, 26, 26],
            "125": [0, 3, 7, 14, 27, 32, 22, 22],
            "160": [0, 5, 8, 15, 29, 42, 21, 21],
            "200": [0, 4, 8, 17, 32, 16, 13, 13],
            "250": [0, 4, 8, 15, 21, 12, 12, 12],
            "315": [0, 4, 7, 13, 21, 12, 13, 13],
            "450": [0, 3, 5, 9, 13, 9, 10, 10],
            "500": [0, 3, 6, 12, 11, 8, 8, 8],
        },
        "2000": {
            "80": [4, 11, 13, 25, 48, 50, 40, 40],
            "100": [0, 3, 11, 24, 46, 49, 32, 32],
            "125": [0, 4, 8, 19, 36, 40, 25, 25],
            "160": [0, 6, 9, 18, 38, 47, 26, 26],
            "200": [0, 5, 10, 22, 42, 22, 17, 17],
            "250": [0, 5, 10, 19, 29, 15, 13, 13],
            "315": [0, 5, 8, 15, 26, 14, 16, 16],
            "450": [0, 5, 6, 11, 17, 11, 11, 11],
            "500": [0, 4, 7, 15, 14, 10, 9, 9],
        },
    },
    "25": {
        "1000": {
            "80": [8, 16, 20, 32, 50, 26, 19, 19],
            "100": [3, 6, 14, 29, 43, 24, 20, 20],
            "125": [2, 9, 16, 29, 31, 14, 11, 11],
            "160": [1, 5, 7, 11, 20, 25, 16, 16],
            "200": [0, 4, 7, 14, 18, 9, 10, 10],
            "250": [0, 6, 9, 15, 15, 8, 11, 11],
            "315": [0, 6, 7, 14, 14, 8, 8, 8],
            "450": [0, 3, 5, 8, 8, 5, 7, 7],
            "500": [0, 3, 7, 10, 7, 6, 7, 7],
        },
        "1500": {
            "80": [8, 21, 26, 41, 50, 37, 26, 26],
            "100": [4, 9, 17, 34, 47, 34, 26, 26],
            "125": [3, 11, 20, 39, 41, 20, 17, 17],
            "160": [2, 9, 13, 22, 35, 29, 19, 19],
            "200": [1, 7, 13, 26, 32, 14, 15, 15],
            "250": [0, 8, 13, 23, 24, 11, 13, 13],
            "315": [0, 5, 10, 20, 19, 11, 12, 12],
            "450": [0, 6, 10, 16, 11, 7, 9, 9],
            "500": [0, 5, 11, 16, 9, 8, 8, 8],
        },
        "2000": {
            "80": [9, 25, 32, 50, 50, 47, 33, 33],
            "100": [4, 11, 20, 38, 50, 44, 31, 31],
            "125": [3, 13, 24, 49, 50, 25, 22, 22],
            "160": [2, 12, 19, 33, 50, 32, 21, 21],
            "200": [1, 10, 19, 37, 45, 19, 19, 19],
            "250": [0, 9, 17, 31, 32, 14, 14, 14],
            "315": [0, 4, 13, 26, 24, 14, 15, 15],
            "450": [0, 9, 14, 23, 13, 8, 11, 11],
            "500": [0, 7, 14, 22, 11, 9, 9, 9],
        },
    }
}

export function calculate (inputs, parameters) {
    const length = parameters["length"];
    const diameter = parameters["diameter"];
    const thickness = parameters["thickness"];

    var output = [];
    for (let freq = 0; freq < 8; freq++) {
        output[freq] = inputs["in1"][freq] - attenuation[thickness][length][diameter][freq];
    }
    return {
        "Lw": output,
        "attenuation": attenuation[thickness][length][diameter]
    };
};

