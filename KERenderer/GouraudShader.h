#pragma once
class GouraudShader
{
};

//for (int i = 0; i < mesh->face.size(); ++i) {
//    kmath::vec3f c[3];
//    for (int j = 0; j < 3; ++j) {
//        c[j] = lightColor * ((lightDir * mesh->normal[mesh->face[i][j].z]));
//        c[j].x = max(0, c[j].x); c[j].x = min(c[j].x, 255);
//        c[j].y = max(0, c[j].y); c[j].y = min(c[j].y, 255);
//        c[j].z = max(0, c[j].z); c[j].z = min(c[j].z, 255);
//    }
//    drawTriangle(mesh->vert[mesh->face[i][0].x],
//        mesh->vert[mesh->face[i][1].x],
//        mesh->vert[mesh->face[i][2].x],
//        mesh->normal[mesh->face[i][0].z],
//        mesh->normal[mesh->face[i][1].z],
//        mesh->normal[mesh->face[i][2].z]
//    );
//}