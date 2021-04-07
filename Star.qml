import QtQuick
import QtQuick3D

import TileEngine

Model {
    id: root
    source: "#Cube"
    materials: [ PrincipledMaterial {
            metalness: 1.0
            roughness: 0.0
            specularAmount: 0.5
            opacity: 1.0

            baseColorMap: Texture { source: "maps/metallic/basecolor.jpg" }
//            metalnessMap: Texture { source: "maps/metallic/metallic.jpg" }
//            roughnessMap: Texture { source: "maps/metallic/roughness.jpg" }
//            normalMap: Texture { source: "maps/metallic/normal.jpg" }
//            metalnessChannel: Material.R
//            roughnessChannel: Material.R
        }
    ]

    property var duration: (50000 * TileView.tile.x) + (50000 * TileView.tile.y) + (50000 * TileView.tile.z)

    SequentialAnimation on eulerRotation {
        loops: Animation.Infinite
        PropertyAnimation {
            duration: Math.max(50000, root.duration)
            from: Qt.vector3d(0, 0, 0)
            to: Qt.vector3d(360, 360, 360)
        }
    }
}
