#include <vector>
#include <map>
#include <string>

class ModelLoader
{
    public:
        ModelLoader();
        ~ModelLoader();

        void LoadFromFile(const char* filename);
        std::vector<float> GetVertexData();
        int GetVertexCount();

    private:
        struct Position { float x, y, z; };
        struct Color { float r, g, b; };
        struct Normal { float x, y, z; };

        void LoadMaterialFile(const char* filename);
        bool StartWith(const std::string& line, const std::string& text);
        void AddVertexData(int vIdx, int nIdx, const char* mtl,
            std::vector<Position>& vertices, std::vector<Normal>& normals);

        std::map<std::string, Color> mMaterialMap;
        std::vector<float> mVertexData;
};