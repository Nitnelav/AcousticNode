export const name = 'CircularDuctWithCoating';

export const caption = 'Circular Duct With Coating';

export const description = 'A simple inplementation of a Circular Duct With Coating attenuation';

export const inputs = [
    { id: "in1", type: 'spectrum', typeName: "Lw", description: 'Input Lw'},
];

export const parameters = [
    { id: "length", type: 'int', typeName: "Int", description: 'Duct Length (mm)', default: 2000 },
    { id: "diameter", type: 'int', typeName: "Int", description: 'Duct Diameter (mm)', default: 200 },
    { id: "thickness", type: 'choice', typeName: "Thickness", description: 'Coating Thickness (mm)', choices: ["12.5", "25"], default: "12.5" }
];

export const outputs = [
    { id: "Lw", type: 'spectrum', typeName: "Lw", description: 'Output Lw' },
];

const coat12p5 = [12, 15, 18, 24, 44, 60, 80, 80]; // extrapolation : 8kHz = 4kHz
const coat25 = [24, 27, 29, 46, 68, 77, 82, 82]; // extrapolation : 8kHz = 4kHz

export function calculate (inputs, parameters) {
    var outputs = {};
    const length = parameters["length"];
    const diameter = parameters["diameter"];
    var coat = [];
    var attenuation = [];

    if (parameters["thickness"] === "12.5") {
        coat = coat12p5;
    } else {
        coat = coat25;
    }
    
    for (let freq = 0; freq < 8; freq++) {
        attenuation[freq] = Math.min(40 * coat[freq] * length * 0.001 / diameter, 25);
    }

    var outputLw = [];
    for (let freq = 0; freq < 8; freq++) {
        outputLw[freq] = inputs["in1"][freq] - attenuation[freq];
    }
    outputs["Lw"] = outputLw;
    return outputs;
};

