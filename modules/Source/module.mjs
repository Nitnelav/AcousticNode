
export const name = 'LwSource';

export const caption = 'Lw Source';

export const inputs = [
];

export const parameters = [
    { type: 'Lw', description: 'Output Lw'},
];

export const outputs = [
    { type: 'Lw', description: 'Output Lw' },
];

export function calculate (inputs, parameters) {
    var outputLw = parameters[0];
    return [ outputLw ];
};

