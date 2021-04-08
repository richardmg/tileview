import QtQuick
import QtQuick3D

Model {
    source: "#Cube"
    scale: Qt.vector3d(0.05, 0.05, 0.05)

    materials: [ PrincipledMaterial {
            metalness: 0.5
            roughness: 0.0
            specularAmount: 1.0
            opacity: 1.0
            baseColorMap: Texture { source: "maps/metallic/basecolor.jpg" }
        }
    ]
}
