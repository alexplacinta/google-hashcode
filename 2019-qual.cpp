#include <bits/stdc++.h>

using namespace std;

#define BIT_SET_SIZE 500

class Hashcode {
public:
    struct Photo {
        char type;
        int n_tags;
//        vector<int> tags;
        bitset<BIT_SET_SIZE> tags;
    };
    struct Slide {
        char type;
        int firstPhoto;
        int secondPhoto;
//        vector<int> tags;
        bitset<BIT_SET_SIZE> tags;
    };

    void solve(ifstream &in, ofstream &out) {
        int n_photos;
        in >> n_photos;
        vector<Photo> photos(n_photos);
        unordered_map<string, int> tagToId;
        unordered_map<int, unordered_map<char, unordered_set<int>>> tagIdToPhotos;
        int tagCounter = 0;
        int totalTags = 0;
        for (int i = 0; i < n_photos; ++i) {
            in >> photos[i].type >> photos[i].n_tags;
            for (int j = 0; j < photos[i].n_tags; ++j) {
                string tag;
                in >> tag;
                if (tagToId.find(tag) == tagToId.end()) {
                    tagCounter++;
                    tagToId.insert({tag, tagCounter});
                    tagToId[tag] = tagCounter;
                    tagIdToPhotos[tagCounter] = {
                            {'H', unordered_set<int>()},
                            {'V', unordered_set<int>()}
                    };
                }
//                photos[i].tags.push_back(tagToId[tag]);
                photos[i].tags.set(tagToId[tag] - 1, true);
                tagIdToPhotos[tagToId[tag]][photos[i].type].insert(i);
            }
//            sort(photos[i].tags.begin(), photos[i].tags.end());
            totalTags += photos[i].n_tags;
        }
        cout << tagCounter << "-" << totalTags << endl;

        vector<Slide> slides;
        set<int> hPhotos;
        set<int> vPhotos;
        for (int i = 0; i < n_photos; ++i) {
            if (photos[i].type == 'H') {
                hPhotos.insert(i);
            } else {
                vPhotos.insert(i);
            }
        }

//        auto unionVPhotoTags = [&](vector<int> &first, vector<int> &second) {
//            set<int> result;
//            for (const auto &i : first) {
//                result.insert(i);
//            }
//            for (const auto &i : second) {
//                result.insert(i);
//            }
//            return vector<int>(result.begin(), result.end());
//        };
        auto unionVPhotoTags = [&](bitset<BIT_SET_SIZE> &first, bitset<BIT_SET_SIZE> &second) {
            return first | second;
        };

//        auto score = [&](vector<int> &a, vector<int> &b) {
//            int intersectCount = 0;
//            int sizeA = a.size();
//            int sizeB = b.size();
//            int iA = 0;
//            int iB = 0;
//            while (iA < sizeA && iB < sizeB) {
//                if (a[iA] == b[iB]) {
//                    intersectCount++;
//                    iA++;
//                    iB++;
//                } else if (a[iA] > b[iB]) {
//                    iB++;
//                } else {
//                    iA++;
//                }
//            }
//
//            return min(min(sizeA - intersectCount, intersectCount), sizeB - intersectCount);
//        };
        auto score = [&](bitset<BIT_SET_SIZE> &a, bitset<BIT_SET_SIZE> &b) {
            int intersectCount = (int)(a & b).count();
            return min({(int)a.count() - intersectCount, intersectCount, (int)b.count() - intersectCount});
        };

//        auto getTags = [&](vector<int>& tags) {
//            return tags;
//        };
        auto getTags = [&](bitset<BIT_SET_SIZE>& tags) {
            vector<int> result;
            for (int i = 0; i < tags.size(); ++i) {
                if (tags[i]) {
                    result.push_back(i+1);
                }
            }
            return result;
        };

//        auto getPosiblePhotos = [&](Slide &slide, char type) {
//            unordered_set<int> posiblePhotos;
//            for (auto& tag : getTags(slide.tags)) {
//                for (auto &photoId : tagIdToPhotos[tag][type]) {
//                    posiblePhotos.insert(photoId);
//                }
//            }
//            return posiblePhotos;
//        };

        auto erasePhoto = [&](int& id) {
            if (photos[id].type == 'H') {
                hPhotos.erase(id);
            } else {
                vPhotos.erase(id);
            }

            for (auto tag : getTags(photos[id].tags)) {
                tagIdToPhotos[tag][photos[id].type].erase(id);
            }
        };

        Slide firstSlide;
        if (hPhotos.size() > 0) {
            int hPhoto = *hPhotos.begin();
            erasePhoto(hPhoto);
            firstSlide.type = photos[hPhoto].type;
            firstSlide.firstPhoto = hPhoto;
            firstSlide.tags = photos[hPhoto].tags;
        } else {
            int firstVPhoto = *vPhotos.begin();
            erasePhoto(firstVPhoto);
            int secondVPhoto = *vPhotos.begin();
            erasePhoto(secondVPhoto);
            firstSlide.type = photos[firstVPhoto].type;
            firstSlide.firstPhoto = firstVPhoto;
            firstSlide.secondPhoto = secondVPhoto;
            firstSlide.tags = unionVPhotoTags(photos[firstVPhoto].tags, photos[secondVPhoto].tags);
        }
        slides.push_back(firstSlide);
        int counter = 0;
        while (!hPhotos.empty() || !vPhotos.empty()) {
            Slide &lastSlide = slides.back();
            int best1stP = 0;
            int bestScore = -1;
//            auto posibleHPhotos = getPosiblePhotos(lastSlide, 'H');
//            auto posibleVPhotos = getPosiblePhotos(lastSlide, 'V');
            auto& posibleHPhotos = hPhotos;
            auto& posibleVPhotos = vPhotos;
            if (posibleHPhotos.size() > 0 || posibleVPhotos.size() > 0) {
                for (int i : posibleHPhotos) {
                    int tempScore = score(lastSlide.tags, photos[i].tags);
                    if (tempScore > bestScore) {
                        bestScore = tempScore;
                        best1stP = i;
                    }
                }
                for (int i : posibleVPhotos) {
                    int tempScore = score(lastSlide.tags, photos[i].tags);
                    if (tempScore > bestScore) {
                        bestScore = tempScore;
                        best1stP = i;
                    }
                }
            } else {
                if (hPhotos.size() > 0) {
                    best1stP = *hPhotos.begin();
                } else {
                    best1stP = *vPhotos.begin();
                }
            }

            erasePhoto(best1stP);
            Slide nextSlide;
            if (photos[best1stP].type == 'H') {
                nextSlide.type = photos[best1stP].type;
                nextSlide.firstPhoto = best1stP;
                nextSlide.tags = photos[best1stP].tags;
                slides.push_back(nextSlide);
            } else {
                if (!vPhotos.empty()) {
                    int best2ndP = 0;
                    int bestScoreV = -1;

//                    auto posibleVPhotos2 = getPosiblePhotos(lastSlide, 'V');
                    auto& posibleVPhotos2 = vPhotos;
                    if (!posibleVPhotos2.empty()) {
                        for (int i : posibleVPhotos2) {
                            auto mergedTags = unionVPhotoTags(photos[best1stP].tags, photos[i].tags);
                            int tempScore = score(lastSlide.tags, mergedTags);
                            if (tempScore > bestScoreV) {
                                bestScoreV = tempScore;
                                best2ndP = i;
                            }
                        }
                    } else {
                        best2ndP = *vPhotos.begin();
                    }


                    erasePhoto(best2ndP);
                    nextSlide.type = photos[best1stP].type;
                    nextSlide.firstPhoto = best1stP;
                    nextSlide.secondPhoto = best2ndP;
                    nextSlide.tags = unionVPhotoTags(photos[best1stP].tags, photos[best2ndP].tags);
                    slides.push_back(nextSlide);
                }

            }
            counter++;
            if (counter % 100 == 0) {
                cout << (hPhotos.size() + vPhotos.size()) << endl;
            }
        }

        int totalScore = 0;
        for (int k = 0; k < slides.size() - 1; ++k) {
            totalScore += score(slides[k].tags, slides[k + 1].tags);
        }
        cerr << totalScore << endl;
//        for (int k = 0; k < slides.size(); ++k) {
//            cout << slides[k].firstPhoto << slides[k].type << endl;
//        }

    }
};

int main() {
    string fileDirectory = "/home/alex/Workspace/google-hashcode/files/";
    string filename = "e_shiny_selfies.txt";

    Hashcode hashcode;
    ifstream in(fileDirectory + "in/" + filename);
    ofstream out(fileDirectory + "out/" + filename);
    const clock_t begin_time = clock();
    hashcode.solve(in, out);
    std::cout << float(clock() - begin_time) / CLOCKS_PER_SEC << endl;
    return 0;
}
