export const name = 'RoomLpFromLwSource';

export const caption = 'Room Lp From Lw Source';

export const description = 'A simple inplementation of Room Lp From Lw Source';

export const inputs = [
    { id: "in1", type: 'spectrum', typeName: "Lw", description: 'Input Lw'},
];

export const parameters = [
    { id: "roomHeight", type: 'float', typeName: "Float", description: 'Room Height (m)', default: 2.5},
    { id: "roomLength", type: 'float', typeName: "Float", description: 'Room Length (m)', default: 5},
    { id: "roomWidth", type: 'float', typeName: "Float", description: 'Room Width (m)', default: 5},
    { id: "distance", type: 'float', typeName: "Float", description: 'Distance Source-Receiver (m)', default: 1},
    { id: "directivity", type: 'choice', typeName: "Choice", description: 'Source Disposition', choices: ["Far from all walls", "Close to 1 wall", "In a 2-wall corner", "In a 3-wall corner"], default: "Far from all walls"},
    { id: "alphaGround", type: 'spectrum', typeName: "Alpha", description: 'Ground Alpha Coefficients', default: [0.01, 0.01, 0.01, 0.02, 0.02, 0.02, 0.03, 0.03]},
    { id: "alphaCeiling", type: 'spectrum', typeName: "Alpha", description: 'Ground Alpha Ceiling', default: [0.01, 0.01, 0.01, 0.02, 0.02, 0.02, 0.03, 0.03]},
    { id: "alphaWall1", type: 'spectrum', typeName: "Alpha", description: 'Ground Alpha Wall1', default: [0.01, 0.01, 0.01, 0.02, 0.02, 0.02, 0.03, 0.03]},
    { id: "alphaWall2", type: 'spectrum', typeName: "Alpha", description: 'Ground Alpha Wall2', default: [0.01, 0.01, 0.01, 0.02, 0.02, 0.02, 0.03, 0.03]},
    { id: "alphaWall3", type: 'spectrum', typeName: "Alpha", description: 'Ground Alpha Wall3', default: [0.01, 0.01, 0.01, 0.02, 0.02, 0.02, 0.03, 0.03]},
    { id: "alphaWall4", type: 'spectrum', typeName: "Alpha", description: 'Ground Alpha Wall4', default: [0.01, 0.01, 0.01, 0.02, 0.02, 0.02, 0.03, 0.03]},
];

export const readonly_outputs = [
];

export const outputs = [
    { id: "out1", type: 'spectrum', typeName: "Lp", description: 'Receiver Lp' },
];

export function calculate (inputs, parameters) {
    const height = parameters["roomHeight"];
    const length = parameters["roomLength"];
    const width = parameters["roomWidth"];
    const distance = parameters["distance"];
    const directivity = parameters["directivity"];
    const alphaGround = parameters["alphaGround"];
    const alphaCeiling = parameters["alphaCeiling"];
    const alphaWall1 = parameters["alphaWall1"];
    const alphaWall2 = parameters["alphaWall2"];
    const alphaWall3 = parameters["alphaWall3"];
    const alphaWall4 = parameters["alphaWall4"];

    const surfaceGround = width * length;
    const surfaceCeiling = width * length;
    const surfaceWall1 = width * height;
    const surfaceWall2 = width * height;
    const surfaceWall3 = height * length;
    const surfaceWall4 = height * length;

    var Q = 1;
    if (directivity == "Far from all walls") {
        Q = 1;
    } else if (directivity == "Close to 1 wall") {
        Q = 2;
    } else if (directivity == "In a 2-wall corner") {
        Q = 4;
    } else if (directivity == "In a 3-wall corner") {
        Q = 8;
    }

    var roomConstant = [];
    for (let freq = 0; freq < 8; freq++) {
        roomConstant[freq] = alphaGround[freq] * surfaceGround
        roomConstant[freq] += alphaCeiling[freq] * surfaceCeiling
        roomConstant[freq] += alphaWall1[freq] * surfaceWall1
        roomConstant[freq] += alphaWall2[freq] * surfaceWall2
        roomConstant[freq] += alphaWall3[freq] * surfaceWall3
        roomConstant[freq] += alphaWall4[freq] * surfaceWall4
    }

    var diffuseField = [];
    for (let freq = 0; freq < 8; freq++) {
        diffuseField[freq] = Math.max(inputs["in1"][freq] + 10 * Math.log10(4 / roomConstant[freq]), 0);
    }

    var directField = [];
    for (let freq = 0; freq < 8; freq++) {
        directField[freq] = Math.max(inputs["in1"][freq] + 10 * Math.log10(Q / (4 * Math.PI * distance * distance)), 0);
    }

    var output = [];
    for (let freq = 0; freq < 8; freq++) {
        output[freq] = Math.max(10 * Math.log10(Math.pow(10, diffuseField[freq] / 10) + Math.pow(10, directField[freq] / 10)), 0);
    }
    return {
        "out1": output
    };
};

