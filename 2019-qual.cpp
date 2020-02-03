#include <bits/stdc++.h>

using namespace std;

#define BIT_SET_SIZE 220

class Hashcode {
public:
    struct Photo {
        char type;
        int n_tags;
        unordered_set<int> tags;
//        bitset<BIT_SET_SIZE> tags;
    };
    struct Slide {
        char type;
        int firstPhoto;
        int secondPhoto;
        unordered_set<int> tags;
//        bitset<BIT_SET_SIZE> tags;
    };

    void solve(ifstream &in, ofstream &out) {
        int n_photos;
        in >> n_photos;
        vector<Photo> photos(n_photos);
        int hCount = 0;
        int vCount = 0;
        unordered_map<string, int> tagToId;
        int tagCounter = 0;
        int totalTags = 0;
        for (int i = 0; i < n_photos; ++i) {
            in >> photos[i].type >> photos[i].n_tags;
            if (photos[i].type == 'H') {
                hCount++;
            } else {
                vCount++;
            }
            for (int j = 0; j < photos[i].n_tags; ++j) {
                string tag;
                in >> tag;
                if (tagToId.find(tag) == tagToId.end()) {
                    tagCounter++;
                    tagToId.insert(make_pair(tag, tagCounter));
                }
                photos[i].tags.insert(tagToId.find(tag)->second);
//                photos[i].tags.set(tagToId.find(tag)->second - 1, true);
            }
            totalTags += photos[i].n_tags;
        }
        cout << tagCounter << "-" << totalTags << endl;

        vector<Slide> slides;
        unordered_set<int> hPhotos;
        unordered_set<int> vPhotos;
        for (int i = 0; i < n_photos; ++i) {
            if (photos[i].type == 'H') {
                hPhotos.insert(i);
            } else {
                vPhotos.insert(i);
            }
        }

        auto unionVPhotoTags = [&](unordered_set<int> &first, unordered_set<int> &second) {
            unordered_set<int> result;
            for (const auto &i : first) {
                result.insert(i);
            }
            for (const auto &i : second) {
                result.insert(i);
            }
            return result;
        };
//        auto unionVPhotoTags = [&](bitset<BIT_SET_SIZE> &first, bitset<BIT_SET_SIZE> &second) {
//            return first | second;
//        };

        auto score = [&](unordered_set<int> &a, unordered_set<int> &b) {
            int intersectCount = 0;
            int aSize = a.size();
            int bSize = b.size();
            if (aSize <= bSize) {
                for (const int &i : a) {
                    if (b.find(i) != b.end()) {
                        intersectCount++;
                    }
                }
            } else {
                for (const int &i : b) {
                    if (a.find(i) != a.end()) {
                        intersectCount++;
                    }
                }
            }

            return min(min(aSize - intersectCount, intersectCount), bSize - intersectCount);
        };
//        auto score = [&](bitset<BIT_SET_SIZE> &a, bitset<BIT_SET_SIZE> &b) {
//            int intersectCount = (int)(a & b).count();
//            return min({(int)a.count() - intersectCount, intersectCount, (int)b.count() - intersectCount});
//        };

        Slide firstSlide;
        if (hCount > 0) {
            int hPhoto = *hPhotos.begin();
            hPhotos.erase(hPhotos.begin());
            firstSlide.type = photos[hPhoto].type;
            firstSlide.firstPhoto = hPhoto;
            firstSlide.tags = photos[hPhoto].tags;
        } else {
            int firstVPhoto = *vPhotos.begin();
            vPhotos.erase(vPhotos.begin());
            int secondVPhoto = *vPhotos.begin();
            vPhotos.erase(vPhotos.begin());
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
            for (int i : hPhotos) {
                int tempScore = score(lastSlide.tags, photos[i].tags);
                if (tempScore > bestScore) {
                    bestScore = tempScore;
                    best1stP = i;
                }
            }
            for (int i : vPhotos) {
                int tempScore = score(lastSlide.tags, photos[i].tags);
                if (tempScore > bestScore) {
                    bestScore = tempScore;
                    best1stP = i;
                }
            }
            Slide nextSlide;
            if (photos[best1stP].type == 'H') {
                hPhotos.erase(best1stP);
                nextSlide.type = photos[best1stP].type;
                nextSlide.firstPhoto = best1stP;
                nextSlide.tags = photos[best1stP].tags;
                slides.push_back(nextSlide);
            } else {
                vPhotos.erase(best1stP);
                if (!vPhotos.empty()) {
                    int best2ndP = 0;
                    int bestScoreV = -1;

                    for (int i : vPhotos) {
                        auto mergedTags = unionVPhotoTags(photos[best1stP].tags, photos[i].tags);
                        int tempScore = score(lastSlide.tags, mergedTags);
                        if (tempScore > bestScoreV) {
                            bestScoreV = tempScore;
                            best2ndP = i;
                        }
                    }
                    vPhotos.erase(best2ndP);

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
        cout << totalScore << endl;
//        for (int k = 0; k < slides.size(); ++k) {
//            cout << slides[k].firstPhoto << slides[k].type << endl;
//        }

    }
};

int main() {
    string fileDirectory = "/home/alex/workspace/task/files/";
    string filename = "b_lovely_landscapes.txt";

    Hashcode hashcode;
    ifstream in(fileDirectory + "in/" + filename);
    ofstream out(fileDirectory + "out/" + filename);
    hashcode.solve(in, out);
    return 0;
}
