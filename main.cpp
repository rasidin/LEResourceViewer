/*********************************************************************
Copyright(c) 2020 LIMITGAME

Permission is hereby granted, free of charge, to any person
obtaining a copy of this softwareand associated documentation
files(the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify,
merge, publish, distribute, sublicense, and /or sell copies of
the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright noticeand this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.
----------------------------------------------------------------------
@file LimitEngine.cpp
@brief Main of LimitEngine
@author minseob (https://github.com/rasidin)
**********************************************************************/
#include <stdio.h>

#include <TBApplication.h>
#include <TBWidget.h>

#include <LEIntVector2.h>

#include <LERenderer>
#include <LimitEngine.h>
#include <Core/MemoryAllocator.h>
#include <Core/SerializableResource.h>
#include <Factories/ArchiveFactory.h>
#include <Renderer/Texture.h>
#include <Renderer/Model.h>

class LimitEngineWindow : public ToolBase::TBWidget
{
public:
    LimitEngineWindow(const char *Title, LimitEngine::LimitEngine *Engine)
        : ToolBase::TBWidget(Title, 1280, 720)
        , mEngine(Engine)
    {}
    ~LimitEngineWindow()
    {}
    bool Load(const char* filename)
    {
        if (LimitEngine::SerializableRendererResource* resource = mEngine->Load(filename, LimitEngine::ArchiveFactory::ID, true)) {
            if (LimitEngine::Texture::IsTextureResource(resource)) {
                mLoadedTexture = static_cast<LimitEngine::Texture*>(resource);
                mLoadedTexture->InitResource();
                mEngine->SetBackgroundImage(mLoadedTexture, LimitEngine::BackgroundImageType::Fullscreen);
            }
            else if (LimitEngine::Model::IsModelResource(resource)) {
                mLoadedModel = static_cast<LimitEngine::Model*>(resource);
            }
            else {
                printf("Unsupported file!!!\n");
                return false;
            }
            return true;
        }
        else return false;
    }
    virtual void FrameUpdate(double frametime) override
    {
        mEngine->Update();
    }
private:
    LimitEngine::LimitEngine* mEngine = nullptr;

    LimitEngine::ReferenceCountedPointer<LimitEngine::Texture>  mLoadedTexture;
    LimitEngine::ReferenceCountedPointer<LimitEngine::Model>    mLoadedModel;
};

void PrintUsage()
{
    printf("LimitEngine V2 Resource Viewer\n");
    printf("by minseob\n");
    printf("\n");
    printf("Usage : LEResourceViewer [lea filepath]\n");
}

enum class ReturnValue
{
    OK = 0,
    InvalidArguments,
    FileNotFound,
};

int main(int argc, const char** argv)
{
    static constexpr size_t TotalMemory = 1024 << 20; // 1 GiB

    if (argc < 2) {
        PrintUsage();
        return static_cast<int>(ReturnValue::InvalidArguments);
    }

    LimitEngine::MemoryAllocator::Init();
    LimitEngine::MemoryAllocator::InitWithMemoryPool(TotalMemory);

    LimitEngine::LimitEngine* engine = new LimitEngine::LimitEngine();

    LimitEngine::InitializeOptions initoptions(
        LEMath::IntSize(1280, 720),
        LimitEngine::InitializeOptions::ColorSpace::Linear,
        LimitEngine::InitializeOptions::ColorSpace::sRGB
    );

    ToolBase::TBApplication app;
    LimitEngineWindow *window = new LimitEngineWindow("Viewer", engine);
    engine->Init(window->GetHandle(), initoptions);

    if (window->Load(argv[1]) == false) {
        printf("File not found!!\n");

        engine->Term();
        delete window;
        delete engine;
        LimitEngine::MemoryAllocator::Term();

        return static_cast<int>(ReturnValue::FileNotFound);
    }
    app.Run();

    engine->Term();
    delete window;

    delete engine;
    LimitEngine::MemoryAllocator::Term();

    return static_cast<int>(ReturnValue::OK);
}