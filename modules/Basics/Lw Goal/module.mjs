export const name = 'LwGoal';

export const caption = 'Lw Goal';

export const inputs = [
    { id: "input", type: 'spectrum', typeName: "Lw", description: 'Input Lw'},
];

export const parameters = [
    { id: "goal", type: 'spectrum', typeName: "Lw", description: 'Lw Goal'},
];

export const outputs = [
];

export function calculate (inputs, parameters) {
    var outputs = {};
    const success = "#00FF00";
    const fail = "#FF0000";
    var ok = true;
    for (let freq = 0; freq < 8; freq++) {
        if (inputs["input"][freq] > parameters["goal"][freq]) {
            ok = false;
        }
    }
    outputs["color"] = ok ? success : fail;
    return outputs;
};

