export const name = 'NRGoal';

export const caption = 'NR Goal';

export const inputs = [
    { id: "input", type: 'int', typeName: "NR", description: 'Input NR'},
];

export const parameters = [
    { id: "goal", type: 'int', typeName: "NR", description: 'NR Goal'},
];

export const readonly_outputs = [
];

export const outputs = [
];

export function calculate (inputs, parameters) {
    const success = "#00FF00";
    const fail = "#FF0000";
    var ok = inputs["input"] < parameters["goal"];
    return {
        "color": ok ? success : fail
    };
};

