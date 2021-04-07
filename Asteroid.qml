import QtQuick
import QtQuick3D

Model {
    source: "asteroid.mesh"
    scale: Qt.vector3d(3.1, 3.1, 3.1)

    materials: [ PrincipledMaterial {
            metalness: 1.0
            roughness: 0.0
            specularAmount: 0.5
            opacity: 1.0
            baseColorMap: Texture { source: "maps/metallic/basecolor.jpg" }
        }
    ]
}
